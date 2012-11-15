/* THIS file mainly contains socket operations. When an AF_MHOST
 * socket is passed any of these messages, they go through these
 * functions before then getting passed to the L4 operators.
 */

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"

#include "udp_table.h"

int mhost_create(struct net *net, struct socket *sock, int protocol, int kern)
{
	struct sock *sk;
    struct inet_sock *inet;
	struct inet_protosw *answer;
    int err;
    char answer_no_check;
    struct proto *answer_prot;
    unsigned char answer_flags;
    printk(KERN_INFO "mhost_create called\n");
    
	sock->state = SS_UNCONNECTED;
    
    /* we only support SOCK_DGRAM as of now... */
    answer = &mhost_dgram_protosw;
    protocol = IPPROTO_UDP;
    /* end "big answer/protocol code mess" here */
    
    err = -EPERM;
    if (sock->type == SOCK_RAW && !kern && !capable(CAP_NET_RAW))
        goto out;

    sock->ops = answer->ops;
    answer_prot = answer->prot;
    answer_no_check = answer->no_check;
    answer_flags = answer->flags;

    WARN_ON(answer_prot->slab == NULL);

    err = -ENOBUFS,
    sk = sk_alloc(net, AF_MHOST, GFP_KERNEL, answer_prot);
    if (sk == NULL)
        goto out;
    
    sock_init_data(sock, sk);

    err = 0;
    sk->sk_no_check = answer_no_check;
    if (INET_PROTOSW_REUSE & answer_flags)
        sk->sk_reuse = 1;
    
    inet = inet_sk(sk);
    inet->is_icsk = (INET_PROTOSW_ICSK & answer_flags) != 0;
    
    if (SOCK_RAW == sock->type) {
        inet->inet_num = protocol;
        if (IPPROTO_RAW == protocol)
            inet->hdrincl = 1;
    }

    sk->sk_destruct    = inet_sock_destruct;
    sk->sk_protocol    = protocol;
    sk->sk_family      = AF_MHOST;
    sk->sk_backlog_rcv = sk->sk_prot->backlog_rcv;

    inet_sk(sk)->pinet6 = (struct ipv6_pinfo *) mhost_sk_generic(sk);
    
    /* HERE DO ALL MHOST-SPECIFIC INITS!!! */
    
    /* now backwards-compat for IPv4 */
    inet->uc_ttl    = -1;
    
    inet->mc_loop   = 1;
    inet->mc_ttl    = 1;
    inet->mc_index  = 0;
    inet->mc_list   = NULL;

    sock_init_data(sock, sk);
            
    inet->uc_ttl    = -1;
    inet->mc_loop   = 1;
    inet->mc_ttl    = 1;
    inet->mc_all    = 1;
    inet->mc_index  = 0;
    inet->mc_list   = NULL;

    if (ipv4_config.no_pmtu_disc)
        inet->pmtudisc = IP_PMTUDISC_DONT;
    else
        inet->pmtudisc = IP_PMTUDISC_WANT;

    sk_refcnt_debug_inc(sk);
    
    if (inet->inet_num) {
        /* assumes that any protocol which allows
         * the user to assign a number at socket
         * creation time automatically shares.
         */
        inet->inet_sport = htons(inet->inet_num);
        sk->sk_prot->hash(sk);
    }

    if (sk->sk_prot->init) {
        int err = sk->sk_prot->init(sk);
        if (err)
            sk_common_release(sk);
    }
    
out:
    return err;
};

int mhost_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *msg, size_t size)
{
    struct sock *sk = sock->sk;
    int err = 0;
    struct sockaddr *sa = NULL;
    
    sock_rps_record_flow(sk);
    
    printk(KERN_INFO "mhost_sendmsg called\n");
    
    /* currently no support for TCP connected state:
    if (sk->sk_state != TCP_ESTABLISHED)
        return -EDESTADDRREQ;
     */
    if (!msg->msg_name) {
        return -EDESTADDRREQ;
    }

    sa = (struct sockaddr *)msg->msg_name;
    
    if (msg->msg_namelen < sizeof(*sa))
        return -EINVAL;
    
    /* table lookup here to confirm that in fact
     * we DO have a L3 handler for this socket! 
     * note that all this function really does
     * is set mhost_sk(sk)->proto appropriately.
     * It may also change the sockaddr values
     * in the event of address-translation
     * (AF_MHOST -> AF_INET, for example).
     */
    err = mhost_translate_sa(sa, sk);
    if (err) {
        printk(KERN_INFO "error: mhost_translate_sa\n");
        return err;
    }
    
    /* We may need to bind the socket */
    if (!inet_sk(sk)->inet_num && !sk->sk_prot->no_autobind &&
        mhost_autobind(sock)) {
        printk(KERN_INFO "error: mhost_autobind\n");
        return -EAGAIN;
    }
    
    err = sk->sk_prot->sendmsg(iocb, sk, msg, size);
    return err;
};

int mhost_bind(struct socket *sock, struct sockaddr *sa, int addr_len)
{
    int err = 0;
    unsigned short snum;
    struct sock *sk = sock->sk;
    struct inet_sock *inet = inet_sk(sk);
    /* mhost not used yet */
//    struct mhost_sock *mhost = mhost_sk(sk);
    struct sockaddr_mhost *sm;
    
    printk(KERN_INFO "mhost_bind called\n");

    /* If the socket has its own bind function then use it! (RAW) */
    if (sk->sk_prot->bind) {
        return sk->sk_prot->bind(sk, sa, addr_len);
    }

    if (addr_len < sizeof(struct sockaddr_mhost))
        return -EINVAL;

    /* how do i feel about different bindings/addressings? 
     * (need to decide how i want to handle these cases)
     */
    if (sa->sa_family != AF_MHOST) {
        /* here, we've specified an address family SO instead
         * of binding across all L3 addresses, we're ONLY going
         * to bind to the specific address family and address
         * specified. Not all L3 protocols may support this behavior
         * and that's okay! I'll add it back in later :-)
         */
        printk(KERN_INFO "error: family not af_mhost\n");
        return -EAFNOSUPPORT;
    }
    
    /* if we've got here, we're binding to AF_MHOST. the rest of
     * the content in the sockaddr is irrelevant (except for the port)
     * because it's meaningless! the goal now is to find an address 
     * that is acceptable across all the different L3 structures. For
     * now, this is all we support so I'm just storing it all in my
     * own UDP table and cross-checking it with the af_inet one!
     */
    
    sm = (struct sockaddr_mhost *)sa;
    
    /* check port validity */
    snum = ntohs(sm->port);
    if (snum && snum < PROT_SOCK && !capable(CAP_NET_BIND_SERVICE)) {
        printk(KERN_INFO "error: port not valid\n");
        return -EACCES;
    }

    lock_sock(sk);

    /* Check these errors (active socket, double bind). */
    if (sk->sk_state != TCP_CLOSE || inet->inet_num) {
        printk(KERN_INFO "error: active socket or double-bind\n");
        err = -EINVAL;
        goto out;
    }
        
    /* these settings are reminiscent to the inet6_bind 
     * case for INADDR_ANY. they correspond to just that! */
    inet->inet_rcv_saddr = 0;
    inet->inet_saddr = 0;
    
    /* inet transport layer binding here! */
    if (sk->sk_prot->get_port(sk, snum)) {
        inet->inet_saddr = inet->inet_rcv_saddr = 0;
        printk(KERN_INFO "error: transport-layer bind failed\n");
        err = -EADDRINUSE;
        goto out;
    }
    
    if (snum)
        sk->sk_userlocks |= SOCK_BINDPORT_LOCK;
    
    inet->inet_sport = htons(inet->inet_num);
    inet->inet_dport = 0;
    inet->inet_daddr = 0;
    
out:
    release_sock(sk);
    return err;
};

int mhost_autobind(struct socket *sock)
{
    struct sock *sk = sock->sk;
    struct inet_sock *inet;
    struct sockaddr_mhost sa;
    sa.sa_family = AF_MHOST;
    sa.port = 0;
    
    /* We may need to bind the socket. */
    inet = inet_sk(sk);   
    if (!inet->inet_num) {
        return mhost_bind(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_mhost));
    }
    
    return 0;
}

int mhost_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *msg, size_t size, int flags)
{
    struct sock *sk = sock->sk;
    int addr_len = 0;
    int err;
    printk(KERN_INFO "mhost_recvmsg called\n");
    
    sock_rps_record_flow(sk);
    
    err = sk->sk_prot->recvmsg(iocb, sk, msg, size, flags & MSG_DONTWAIT,
                               flags & ~MSG_DONTWAIT, &addr_len);
    if (err >= 0)
        msg->msg_namelen = addr_len;
    return err;
};

/* untested */

int mhost_dgram_connect(struct socket *sock, struct sockaddr * uaddr, int addr_len, int flags)
{
    struct sock *sk = sock->sk;
    printk(KERN_INFO "mhost_dgram_connect called\n");
    
    
    if (addr_len < sizeof(struct sockaddr_mhost))
        return -EINVAL;
    
    /*
     if (uaddr->sa_family == AF_UNSPEC)
     return sk->sk_prot->disconnect(sk, flags);
     */
    
    if (!inet_sk(sk)->inet_num && mhost_autobind(sock)) {
        printk(KERN_INFO "error: already bound\n");
        return -EAGAIN;
    }
    
    return sk->sk_prot->connect(sk, uaddr, addr_len);
};

int mhost_release(struct socket *sock)
{
    int err;
    struct sock *sk = sock->sk;

    printk(KERN_INFO "mhost_release called\n");

    /* part of the cleanup operation */
    udp_table_remove(sk);
    
    err = inet_release(sock);
    if (err) {
        printk(KERN_INFO "error: inet_release returned %d\n", err);
        return err;
    }
    
    return 0;
};

/* functions yet to be written */

int mhost_shutdown(struct socket *sock, int how)
{
    printk(KERN_INFO "mhost_shutdown called\n");
    return 0;
};

int mhost_getname(struct socket *sock, struct sockaddr *uaddr, int *uaddr_len, int peer)
{
    printk(KERN_INFO "mhost_getname called\n");
    return 0;
};

int mhost_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
    struct sock *sk = sock->sk;
    int err = 0;
    printk(KERN_INFO "mhost_ioctl called\n");
    
    switch (cmd) {
            /*
             case SIOCGSTAMP:
             err = sock_get_timestamp(sk, (struct timeval __user *)arg);
             break;
             */
        default:
            if (sk->sk_prot->ioctl)
                err = sk->sk_prot->ioctl(sk, cmd, arg);
            else
                err = -ENOIOCTLCMD;
            break;
    }
    return err;
};

ssize_t mhost_sendpage(struct socket *sock, struct page *page, int offset, size_t size, int flags)
{
    struct sock *sk = sock->sk;
    printk(KERN_INFO "mhost_sendpage called\n");
    
    sock_rps_record_flow(sk);
        
    if (sk->sk_prot->sendpage) {
        return sk->sk_prot->sendpage(sk, page, offset, size, flags);
    }
    return sock_no_sendpage(sock, page, offset, size, flags);
};

/* functions added only for autobind testing */

int mhost_rcv_saddr_equal(const struct sock *sk1, const struct sock *sk2)
{
    struct inet_sock *inet1 = inet_sk(sk1), *inet2 = inet_sk(sk2);
    
    return  (!ipv6_only_sock(sk2)  &&
             (!inet1->inet_rcv_saddr || !inet2->inet_rcv_saddr ||
              inet1->inet_rcv_saddr == inet2->inet_rcv_saddr));
}
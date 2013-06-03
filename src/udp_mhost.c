#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"

int udpmhost_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len)
{
    struct mhost_sock *ms = mhost_sk(sk);
    struct inet_sock *inet = inet_sk(sk);
    struct sockaddr_mhost *sa;
//    struct udp_mhost_sock *up = udp_mhost_sk(sk);
    struct iovec *iov = msg->msg_iov;
    struct udphdr *hdr;
    struct sk_buff *skb;
    int err = 0;
    int ulen = len + sizeof(struct udphdr);
    __be16 dport = 0;
    unsigned char *data;
    
    printk(KERN_INFO "udpmhost_sendmsg called\n");
    
    /* shortcut for the case where L3 interacts closely with L4 
     * NOTE: IPv4 and IPv6 both use this! */
    if (ms->proto->udp_sendmsg) {
        printk(KERN_INFO "SMS: calling udp_sendmsg\n");
        err = ms->proto->udp_sendmsg(iocb, sk, msg, len);
        if (err != len) {
            printk(KERN_INFO "error: %d\n", err);
        }
        return err;
    }
    
    /* early error-checking */
    if (len > 0xFFFF)
        return -EMSGSIZE;
    if (msg->msg_flags & MSG_OOB)
        return -EOPNOTSUPP;
    
    sa = (struct sockaddr_mhost *)msg->msg_name;
    dport = sa->port;
    /* NOT DONE YET!!! */
    
    /* create an skb and copy the udp header and payload */
    skb = alloc_skb(UDP_MAX_HEADER + ulen, sk->sk_allocation);
    if (unlikely(skb == NULL))
        return -ENOBUFS;
    skb_reserve(skb, UDP_MAX_HEADER);
    
    /* copy payload from userspace */
    data = skb_put(skb, len);
	skb->csum = csum_and_copy_from_user(iov->iov_base, data, len, 0, &err);
	if (err) {
//         log?
//         free skb? 
        return -err;
    }
        
    /* generate udp header here */
	hdr = (struct udphdr *)skb_push(skb, sizeof(struct udphdr));

    hdr->source = htons(inet->inet_sport);
    hdr->len = htons(ulen);
    hdr->dest = htons(dport);
    hdr->check = 0;
        
    /* pass packet to l3 handler */
    lock_sock(sk);
    err = ms->proto->sendmsg(sk, skb, (struct sockaddr *)sa, ulen);
    release_sock(sk);
    
    if (!err)
        return len;

    /* NOT DONE YET!!! */
    return err;
};

int udpmhost_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len, int noblock, int flags, int *addr_len)
{
    struct sockaddr_mhost *sin = (struct sockaddr_mhost *)msg->msg_name;
    struct sk_buff *skb;
    unsigned int ulen;
    int peeked;
    int err;
    int is_udplite = IS_UDPLITE(sk);
    
    printk(KERN_INFO "udpmhost_recvmsg called\n");

    /*
     *      Check any passed addresses
     */
    if (addr_len)
        *addr_len = sizeof(*sin);
    
    /* if (flags & MSG_ERRQUEUE) 
     * return mhost_recv_error(sk, msg, len) */
    skb = __skb_recv_datagram(sk, flags | (noblock ? MSG_DONTWAIT : 0),
                              &peeked, &err);
    if (!skb)
        goto out;
    
    ulen = skb->len - sizeof(struct udphdr);
    if (len > ulen)
        len = ulen;
    else if (len < ulen)
        msg->msg_flags |= MSG_TRUNC;
        
    /* we don't support csums yet! */
    err = skb_copy_datagram_iovec(skb, sizeof(struct udphdr),
                                  msg->msg_iov, len);

    if (err)
        goto out_free;
    
    if (!peeked)
        UDP_INC_STATS_USER(sock_net(sk),
                           UDP_MIB_INDATAGRAMS, is_udplite);
    
    sock_recv_timestamp(msg, sk, skb);

    if (sin) {
        sin->sa_family = AF_MHOST;
        sin->port = udp_hdr(skb)->source;
        memset(sin->opaque, 0, sizeof(sin->opaque));
    }
//    if (inet->cmsg_flags)
//        mhost_cmsg_recv(msg, skb);
    
    err = len;
    if (flags & MSG_TRUNC)
        err = ulen;
    
out_free:
    skb_free_datagram_locked(sk, skb);
out:
    return err;
};

void udp_destroy_sock(struct sock *sk)
{
    lock_sock(sk);
    udp_flush_pending_frames(sk);
    release_sock(sk);
};

/*  THIS was my hacky re-write of udp_v4_get_port. It
 *  actually makes sense to just call udp_v6_get_port.
 */
/*
int udp_mhost_get_port(struct sock *sk, unsigned short snum)
{
    int retval = 0;    
    printk(KERN_INFO "udp_mhost_get_port called\n");
    retval = udp_lib_get_port(sk, snum, mhost_rcv_saddr_equal);

     * store udp port in a table! 
     * note that we use the original udp get port function
     * so that we're guaranteed that all mhost ports are also
     * reserved and valid for INET as well 

    if (!retval) {
        udp_table_insert(sk, inet_sk(sk)->inet_num);
    }
    
    return retval;
} 
*/

unsigned int udp4_portaddr_hash(struct net *net, __be32 saddr, unsigned int port)
{
    return jhash_1word((__force u32)saddr, net_hash_mix(net)) ^ port;
}

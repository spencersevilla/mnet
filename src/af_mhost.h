#ifndef _AF_MHOST_H
#define _AF_MHOST_H

#include "kernel_includes.h"

#define AF_MHOST 27

#define MHOST_HEADER_SIZE 48
#define MHOST_MAX_HEADER MHOST_HEADER_SIZE + LL_MAX_HEADER

/* userspace exposed */
struct sockaddr_mhost {
    short           sa_family;
    unsigned short  port;
    char            opaque[12];
};

/* kernel-level exposed */
struct mhost_proto {
    short family;
    struct module *owner;
    struct mhost_proto *next;
    
    /* IPv4/IPv6 hook-in wrapper */
    int (*udp_sendmsg) (struct kiocb *iocb, struct sock *sk,
                            struct msghdr *msg, size_t len);
    int (*sendmsg) (struct sock *sk, struct sk_buff *skb, int len);
    int (*rcv) (struct sk_buff *skb, struct net_device *dev, 
                    struct net_device *orig_dev);
};

/* this field no longer has to contain sock, inet, etc.
 * because it resides at the VERY END of the data allocated!!!
 * (note that it's accessible via the pinet6 pointer in inet_sock)
 */
struct mhost_sock {
    struct mhost_proto *proto;
};

/* defined in af_mhost.c */
extern const struct net_proto_family mhost_family_ops;
extern const struct proto_ops mhost_dgram_ops;

/* static functions */
static inline struct mhost_sock * mhost_sk_generic(struct sock *sk)
{
    const int offset = sk->sk_prot->obj_size - sizeof(struct mhost_sock);
    
    return (struct mhost_sock *)(((u8 *)sk) + offset);
}

static inline struct mhost_sock * mhost_sk(const struct sock *sk)
{
    return (struct mhost_sock *) inet_sk(sk)->pinet6;
}

/* FUNCTIONS FUNCTIONS FUNCTIONS!!! */
int mhost_create(struct net *net, struct socket *sock, int protocol, int kern);
int mhost_release(struct socket *sock);
int mhost_bind(struct socket *sock, struct sockaddr *sa, int addr_len);
int mhost_dgram_connect(struct socket *sock, struct sockaddr * uaddr, int addr_len, int flags);
int mhost_getname(struct socket *sock, struct sockaddr *uaddr, int *uaddr_len, int peer);
int mhost_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg);
int mhost_shutdown(struct socket *sock, int how);
int mhost_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *msg, size_t size);
int mhost_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *msg, size_t size, int flags);
ssize_t mhost_sendpage(struct socket *sock, struct page *page, int offset, size_t size, int flags);
int mhost_init(void);
int mhost_autobind(struct socket *sock);
struct sk_buff *mhost_make_skb(struct sock *sk, void *from, int length, int hdrlen, unsigned int flags);
int mhost_rcv_saddr_equal(const struct sock *sk1, const struct sock *sk2);
#endif /* _AF_MHOST_H */
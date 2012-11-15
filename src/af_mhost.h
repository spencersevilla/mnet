#ifndef _AF_MHOST_H
#define _AF_MHOST_H

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"

/* functions defined in af_mhost.c */
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
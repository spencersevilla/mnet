#ifndef _MHOST_FUNCS_H
#define _MHOST_FUNCS_H

/* This file contains functions that are called outside of the files they're in */

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
int mhost_autobind(struct socket *sock);
struct sk_buff *mhost_make_skb(struct sock *sk, void *from, int length, int hdrlen, unsigned int flags);
int mhost_rcv_saddr_equal(const struct sock *sk1, const struct sock *sk2);

/* defined in udp_mhost.c */
int udpmhost_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len);
int udpmhost_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len, int noblock, int flags, int *addr_len);
int udp_mhost_get_port(struct sock *sk, unsigned short snum);
int udp_mhost_rcv(struct sk_buff *skb);

/* defined in udp_table.c */
int udp_table_insert(struct sock *sk, int port);
int udp_table_remove(struct sock *sk);
struct sock * udp_table_lookup(int port);

/* in net/ipv4/udp_impl.h */
void udp_destroy_sock(struct sock *sk);
unsigned int udp4_portaddr_hash(struct net *net, __be32 saddr, unsigned int port);

/* defined in mhost_table.c */
int mhost_translate_sa(struct sockaddr *sa, struct sock *sk);
int mhost_table_register(struct mhost_proto *proto);
int insert_sockaddr_id(struct sockaddr *sa, short id);
struct mhost_proto * mhost_proto_for_family(short family);

#endif

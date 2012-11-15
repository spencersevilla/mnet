#ifndef _UDP_MHOST_H
#define _UDP_MHOST_H

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"

/* in net/ipv4/udp_impl.h */
void udp_destroy_sock(struct sock *sk);
unsigned int udp4_portaddr_hash(struct net *net, __be32 saddr, unsigned int port);

int udpmhost_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len);
int udpmhost_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len, int noblock, int flags, int *addr_len);
int udp_mhost_get_port(struct sock *sk, unsigned short snum);

#endif /* _UDP_MHOST_H */
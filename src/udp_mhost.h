#ifndef _UDP_MHOST_H
#define _UDP_MHOST_H

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "af_mhost.h"

extern struct proto udpmhost_prot;
extern struct inet_protosw mhost_dgram_protosw;
extern struct inet_protosw fake_dgram_protosw;

/* in net/ipv4/udp_impl.h */
void udp_destroy_sock(struct sock *sk);
unsigned int udp4_portaddr_hash(struct net *net, __be32 saddr, unsigned int port);

int udpmhost_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len);
int udpmhost_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len, int noblock, int flags, int *addr_len);
int udp_mhost_get_port(struct sock *sk, unsigned short snum);
int udp_mhost_rcv(struct sk_buff *skb);

#endif /* _UDP_MHOST_H */
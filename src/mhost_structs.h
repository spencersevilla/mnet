#ifndef _MHOST_STRUCTS_H
#define _MHOST_STRUCTS_H

#include "kernel_includes.h"

/* USER-SPACE EXPOSED =============================== */

struct sockaddr_mhost {
    short           sa_family;
    unsigned short  port;
    unsigned short  id_no;
    char            opaque[10];
};



/* KERNEL-LEVEL EXPOSED ============================= */

struct mhost_proto {
    short family;
    struct module *owner;
    struct mhost_proto *next;
    
    /* IPv4/IPv6 hook-in wrapper */
    int (*udp_sendmsg) (struct kiocb *iocb, struct sock *sk,
                        struct msghdr *msg, size_t len);
    int (*sendmsg) (struct sock *sk, struct sk_buff *skb,
                    struct sockaddr *sa, int len);
    int (*rcv) (struct sk_buff *skb, struct net_device *dev,
                struct net_device *orig_dev);
};

/* this field resides at the VERY END of the data allocated!!!
 * (note that it's accessible via the pinet6 pointer in inet_sock)
 */
struct mhost_sock {
    struct ipv6_pinfo ip6;
    struct mhost_proto *proto;
};

struct udp_mhost_sock {
    struct udp_sock udp;
    /* struct mhost_sock must be last member:
     * basically, we're copying the ipv6 model
     * and overloading the ipv6 pointer */
    struct mhost_sock ms;
};



/* STRUCTURES THAT ARE USED ACROSS FILES =============== */

/* defined in af_mhost.c */
extern const struct net_proto_family mhost_family_ops;
extern const struct proto_ops mhost_dgram_ops;

/* defined in mhost_inet.c */
extern struct mhost_proto inet_mhost_proto;
extern struct mhost_proto inet6_mhost_proto;
extern struct sockaddr_in test_sa;

/* defined in mhost_l3_wrapper.c */
extern struct packet_type mhost_ptype;

/* defined in udp_mhost.c */
extern struct proto udpmhost_prot;
extern struct inet_protosw mhost_dgram_protosw;
extern struct inet_protosw fake_dgram_protosw;

/* STRUCT CASTING FUNCTIONS ============================= */
static inline struct mhost_sock * mhost_sk_generic(struct sock *sk)
{
    const int offset = sk->sk_prot->obj_size - sizeof(struct mhost_sock);
    
    return (struct mhost_sock *)(((u8 *)sk) + offset);
}

static inline struct mhost_sock * mhost_sk(const struct sock *sk)
{
    return (struct mhost_sock *) inet_sk(sk)->pinet6;
}

static inline struct udp_mhost_sock *udp_mhost_sk(const struct sock *sk)
{
    return (struct udp_mhost_sock *)sk;
}

#endif

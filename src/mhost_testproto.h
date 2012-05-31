#ifndef _mhost_testproto_h
#define _mhost_testproto_h

#define AF_TESTPROTO 1

#include "kernel_includes.h"

extern struct mhost_proto test_mhost_proto;

/* routing-layer specific */
struct testhdr {
    __be16 family; /* MUST BE FIRST FIELD!!! */
    __be16 ones;   /* all ones for test here */
};

int test_mhost_init(void);
int test_mhost_sendmsg(struct sock *sk, struct sk_buff *skb, int len);
int test_mhost_rcv(struct sk_buff *skb, struct net_device *dev, struct net_device *orig_dev);

#endif

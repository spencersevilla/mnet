//
//  mhost_otherproto.h
//  
//
//  Created by spencer on 6/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _mhost_otherproto_h
#define _mhost_otherproto_h

#define AF_OTHERPROTO 2

#include "kernel_includes.h"

extern struct mhost_proto other_mhost_proto;

/* routing-layer specific */
struct otherhdr {
    __be16 family; /* MUST BE FIRST FIELD!!! */
    __be16 zeroes;   /* all zeroes for test here */
};

int other_mhost_init(void);
int other_mhost_sendmsg(struct sock *sk, struct sk_buff *skb, int len);
int other_mhost_rcv(struct sk_buff *skb, struct net_device *dev, struct net_device *orig_dev);

#endif

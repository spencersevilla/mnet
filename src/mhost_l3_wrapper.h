#ifndef _MHOST_L3_WRAPPER_H
#define _MHOST_L3_WRAPPER_H

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_funcs.h"

extern struct packet_type mhost_ptype;

/* TESTING: remove these later! */
extern char baddr[6];
extern char daddr1[6];
extern char daddr2[6];

/* downstack */
int mhost_finish_output(struct sk_buff *skb, struct net_device *dev, const void *daddr);

/* upstack */
int mhost_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev);
int mhost_local_deliver(struct sk_buff *skb);

/* ??? */
int mhost_gso_send_check(struct sk_buff *skb);
struct sk_buff *mhost_gso_segment(struct sk_buff *skb, u32 features);
struct sk_buff **mhost_gro_receive(struct sk_buff **head, struct sk_buff *skb);
int mhost_gro_complete(struct sk_buff *skb);

#endif

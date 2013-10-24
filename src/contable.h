#ifndef _CONTABLE_H
#define _CONTABLE_H

#include "kernel_includes.h"
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

// hook function to register with netfilter
unsigned int out_hook(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *));

unsigned int in_hook(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *));

unsigned int lookup_hostid(unsigned int hid);
unsigned int lookup_srcid(unsigned int sid);
#endif

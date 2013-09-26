#include "contable.h"

unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{
	printk(KERN_INFO "NETFILTER!!!\n");
	return NF_ACCEPT;
}

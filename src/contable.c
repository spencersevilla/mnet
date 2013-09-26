#include "contable.h"

unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{

	struct iphdr *iph = ip_hdr(skb);
	if (iph.daddr == 0x0100007F) {
		printk(KERN_INFO "NETFILTER!!!\n");
	}
	return NF_ACCEPT;
}

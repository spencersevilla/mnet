#include "contable.h"

unsigned int out_hook(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{

	struct iphdr *iph = ip_hdr(skb);
	if (!iph) {
		return NF_ACCEPT;
	}

	if (iph->daddr == 0x08080808) {
//		iph->daddr = 0x08080808;
//		iph->saddr = 0x4B3B7280;
		printk(KERN_INFO "NETFILTER OUT TO 8.8.8.8!!! \n");
	}
	return NF_ACCEPT;
}

unsigned int in_hook(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{

	struct iphdr *iph = ip_hdr(skb);
	if (!iph) {
		return NF_ACCEPT;
	}

	if (iph->saddr == 0x08080808) {
//		iph->daddr = 0x0100007F;
//		iph->saddr = 0x01010101;
		printk(KERN_INFO "NETFILTER IN FROM 8.8.8.8!!! \n");
	}
	return NF_ACCEPT;
}

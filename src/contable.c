#include "contable.h"

unsigned int out_hook(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{
	struct iphdr *iph;
	unsigned int hostid;
	unsigned int daddr;

	iph = ip_hdr(skb);
	if (!iph) {
		return NF_ACCEPT;
	}

	hostid = iph->daddr;
	// table lookup function!!!
	daddr = lookup_hostid(hostid);

	// 0 means the address is a failure, so abort!
	// daddr == hostid means we didn't get a table-hit
	// anything else means we've got an address..!
	if (daddr == 0) {
		printk(KERN_INFO "contable error: invalid hostid %d", hostid);
		return NF_DROP;
	}

	iph->daddr = daddr;
	// might also have to update saddr and net_device???
	return NF_ACCEPT;
}

unsigned int in_hook(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{
	struct iphdr *iph;
	unsigned int hostid;
	unsigned int saddr;

	iph = ip_hdr(skb);
	if (!iph) {
		return NF_ACCEPT;
	}

	saddr = iph->saddr;
	// table lookup function!!!
	hostid = lookup_srcid(saddr);

	// 0 means the address is a failure somehow, so abort!
	// anything else means we've got a hostid..!
	if (hostid == 0) {
		printk(KERN_INFO "contable error: invalid saddr %d", saddr);
		return NF_DROP;
	}

	iph->saddr = hostid;
	// might have to update other fields???
	return NF_ACCEPT;
}

unsigned int lookup_hostid(unsigned int hid) {
	if (hid == 0x08080808) {
		printk(KERN_INFO "NETFILTER OUT TO 8.8.8.8!!! \n");
	}
	return hid;
}

unsigned int lookup_srcid(unsigned int sid) {
	if (sid == 0x08080808) {
		printk(KERN_INFO "NETFILTER IN FROM 8.8.8.8!!! \n");
	}
	return sid;
}
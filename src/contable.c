#include "contable.h"

static unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb,
			const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{
	return NF_ACCEPT;
}
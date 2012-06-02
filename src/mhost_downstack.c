#include "mhost_downstack.h"
#include "mhost_upstack.h"

/* master function called by L3 
 * analagous to ip_output and ip_finish_output{2}
 * they then call the equivalent of neigh_connected_output
 * A LOT of arp shit happens here too!
 * for reference: Figure 27.13
 */

/* 802.11 broadcast address... useful to have lying around! */
char baddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int mhost_finish_output(struct sk_buff *skb, struct net_device *dev, const void *daddr)
{
    int err = 0;
    
    printk(KERN_INFO "mhost_finish_output called\n");
    
    /* set device-specific header here... */
    skb->protocol = htons(ETH_P_MHOST);
    skb->dev = dev;
    err = dev_hard_header(skb, dev, ntohs(skb->protocol),
                          daddr, NULL, skb->len);
    
    /* ...and ship off to the device driver! */
    if (err >= 0) {
        err = dev_queue_xmit(skb);
        if (err < 0) {
            printk(KERN_INFO "error: dev_queue_xmit!\n");
        }
    }
    else {
        printk(KERN_INFO "error: dev_hard_header returned %d\n", err);
        err = -EINVAL;
        kfree_skb(skb);
    }
    return err;
}

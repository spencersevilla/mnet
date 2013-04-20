#include "mhost_l3_wrapper.h"

struct packet_type mhost_ptype = {
    .type = cpu_to_be16(ETH_P_MHOST),
    .func = mhost_rcv,
    .gso_send_check = mhost_gso_send_check,
    .gso_segment = mhost_gso_segment,
    .gro_receive = mhost_gro_receive,
    .gro_complete = mhost_gro_complete,
};

struct l3_hdr {
    short family;
};

/* 802.11 broadcast address... useful to have lying around! */
char baddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* SMS: remove later, these for testing! */
char daddr1[6] = {0x20, 0x10, 0x7a, 0x0b, 0x48, 0x65};
char daddr2[6] = {0x20, 0x10, 0x7a, 0x0b, 0x48, 0x84};
char daddr3[6] = {0x20, 0x10, 0x7a, 0x12, 0x85, 0x9a};
/* (end remove) */

/* DOWNSTACK: MASTER FUNCTION CALLED BY L3
 * analagous to ip_output and ip_finish_output{2}
 * they then call the equivalent of neigh_connected_output
 * A LOT of arp shit happens here too!
 * for reference: Figure 27.13
 */
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

/* UPSTACK: MASTER FUNCTION CALLED BY L2 */
int mhost_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
    struct mhost_proto *mp;
    struct l3_hdr *hdr;
    
    printk(KERN_INFO "mhost_rcv called\n");
    
    /* error-checking here */
    
    skb_set_network_header(skb, 0);
    
    /* address family MUST be first member of L3 header 
     * so that we can quickly perform a table lookup on it.*/
    hdr = (struct l3_hdr *) skb_network_header(skb);
    mp = mhost_proto_for_family(hdr->family);

    /* ...and pass it up the stack! */
    if (mp && mp->rcv) {
        mp->rcv(skb, dev, orig_dev);
        return 0;
    }
    
    printk(KERN_INFO "error: no L3 handler registered! family=%x\n", hdr->family);
    return -EAFNOSUPPORT;
}

int mhost_local_deliver(struct sk_buff *skb)
{
    printk(KERN_INFO "mhost_local_deliver called\n");
    
    /* update this pointer */
    skb_set_transport_header(skb, 0);

    /* logic here to determine appropriate L4 handler */
    
    udp_mhost_rcv(skb);
    return 0;
}

/* corresponds to __udp4_lib_rcv */
int udp_mhost_rcv(struct sk_buff *skb)
{
    int ret;
    struct sock *sk;
    struct udphdr *uh;
    unsigned short ulen;
    int saddr, daddr;
    printk(KERN_INFO "udp_mhost_rcv called\n");

//    if (!pskb_may_pull(skb, sizeof(struct udphdr)))
//        goto drop;

    uh = (struct udphdr *) skb_transport_header(skb);
    ulen = ntohs(uh->len);
    
    // SMS: FIND A BETTER FIX FOR THIS!!!
    skb->len = ulen;
    
    // eh?
    saddr = 0;
    daddr = 0;
    
    //    INT CORRESPONDS TO dev_get_by_index OR dev->ifindex!!!
    /* first we need to lookup the socket... */
//    sk = udp4_lib_lookup(dev_net(skb->dev), saddr, uh->source, daddr, uh->dest, (skb->dev)->ifindex);

    sk = udp_table_lookup(uh->dest);
    
    /* check for multicast here */
    
    if (sk != NULL) {
        ret = sock_queue_rcv_skb(sk, skb);
        if (ret < 0) {
            kfree_skb(skb);
        }
        return ret;
    }
    
    /* no socket wants it, so drop silently */
    printk(KERN_INFO "error: sk NULL!\n");
    kfree_skb(skb);
    return 0;
}

/* not yet written/understood! */

int mhost_gso_send_check(struct sk_buff *skb)
{
    printk(KERN_INFO "mhost_gso_send_check called\n");
    return 0;
}

struct sk_buff *mhost_gso_segment(struct sk_buff *skb, int features)
{
    printk(KERN_INFO "mhost_gso_segment called\n");
    return NULL;
}

struct sk_buff **mhost_gro_receive(struct sk_buff **head, struct sk_buff *skb)
{
    printk(KERN_INFO "mhost_gro_receive called\n");
    return NULL;
}

int mhost_gro_complete(struct sk_buff *skb)
{
    printk(KERN_INFO "mhost_gro_complete called\n");
    return 0;
}
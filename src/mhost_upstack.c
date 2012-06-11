#include "mhost_upstack.h"
#include "mhost_table.h"
#include "udp_mhost.h"
#include "udp_table.h"

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

/* master function called by L2 */
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

struct sk_buff *mhost_gso_segment(struct sk_buff *skb, u32 features)
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
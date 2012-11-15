#include "mhost_otherproto.h"
#include "mhost_upstack.h"
#include "mhost_downstack.h"

/* this should be only structure needed to interact with mhost */
struct mhost_proto other_mhost_proto = {
    .family         = AF_OTHERPROTO,
    .owner          = THIS_MODULE,
    .next           = NULL,
    .sendmsg        = other_mhost_sendmsg,
    .rcv            = other_mhost_rcv,
};

/* function called when module initialized */
int other_mhost_init(void)
{
    printk(KERN_INFO "other_mhost_init called\n");
    mhost_table_register(&other_mhost_proto);
    
    /* initialize any specific routing table stuff here */
    
    return 0;
}

int other_mhost_sendmsg(struct sock *sk, struct sk_buff *skb, struct sockaddr *sa, int len)
{
    struct otherhdr *hdr;
    struct net_device *dev = NULL;
    char *daddr;
    struct sockaddr_mhost *sm = (struct sockaddr_mhost *)sa;
    
    printk(KERN_INFO "other_mhost_sendmsg called\n");
    
    /* do routing work to find a device */
    if (sm->sa_family != AF_OTHERPROTO) {
        printk(KERN_INFO "error: wrong sockaddr type!\n");
        return -1;
    }
    
    /* really intelligent routing! */
    if (sm->id_no == 0) {
        printk(KERN_INFO "id_no == 0\n");
        dev = (sock_net(sk))->loopback_dev;
        daddr = NULL;
    } else {
        dev = dev_get_by_index(sock_net(sk), 3);
        if (!dev) {
            printk(KERN_INFO "error: dev not found!\n");
            dev = (sock_net(sk))->loopback_dev;
        }
        if (sm->id_no == 1) {
            daddr = daddr1;
        } else {
            daddr = daddr2;
        }
    }

    /* build header */
    hdr = (struct otherhdr *)skb_push(skb, sizeof(struct otherhdr));
    hdr->family = AF_OTHERPROTO;
    hdr->zeroes = 0x0000;
    
    printk(KERN_INFO "sending to: [%p:%s]\n", dev, dev->name);
    
    /* send down the stack! */
    /* NOTE: dst must be set if you want to use an actual interface! */
    return mhost_finish_output(skb, dev, daddr);
};

int other_mhost_rcv(struct sk_buff *skb, struct net_device *dev, 
                   struct net_device *orig_dev)
{
    struct otherhdr *hdr;
    
    printk(KERN_INFO "other_mhost_rcv called\n");
    
    /* preserve network_header location */
    skb_pull(skb, sizeof(struct otherhdr));
    hdr = (struct otherhdr *) skb_network_header(skb);
    
    if (hdr->zeroes != 0x0000) {
        printk(KERN_INFO "error: hdr->ones not all ones!\n");
    }
    
    mhost_local_deliver(skb);
    return 0;
}

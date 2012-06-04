#include "af_mhost.h"
#include "udp_mhost.h"
#include "mhost_table.h"
#include "mhost_inet.h"
#include "mhost_upstack.h"
#include "mhost_testproto.h"
#include "mhost_otherproto.h"
#include "kernel_includes.h"

int init_module(void)
{
    int retval;
    printk(KERN_INFO "adding mobile_host module\n");
	retval = mhost_init();
    return retval;
}

void cleanup_module(void)
{
	printk(KERN_INFO "removing mobile_host module\n");
}

int mhost_init(void)
{
    int rc = -EINVAL;
	printk(KERN_INFO "initializing AF_MHOST family\n");
    
    rc = proto_register(&udpmhost_prot, 1);
    if (rc) {
        printk(KERN_INFO "err: udpmhost_prot not registered\n");
        goto out;
    }

	sock_register(&mhost_family_ops);
    dev_add_pack(&mhost_ptype);
    
    /* my table functions here */
    mhost_table_register(&inet_mhost_proto);
    test_mhost_init();
    other_mhost_init();
    
    /* this function populates a-priori knowledge of rts */
    table_sim_init();
    
    rc = 0;
out:
    return rc;
};

MODULE_LICENSE("GPL");

c#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"

int sim_init_1(void);
int sim_init_2(void);
int sim_init_3(void);
int mhost_init(void);

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
    
    proto_unregister(&udpmhost_prot);
    sock_unregister(AF_MHOST);
    dev_remove_pack(&mhost_ptype);
}

int mhost_init(void)
{
    int res;
    struct socket * sock;
    struct proto * udpv6_prot;

    int rc = -EINVAL;
	printk(KERN_INFO "initializing AF_MHOST family\n");
    
    rc = proto_register(&udpmhost_prot, 1);
    if (rc) {
        printk(KERN_INFO "err: udpmhost_prot not registered\n");
        goto out;
    }

	sock_register(&mhost_family_ops);
    dev_add_pack(&mhost_ptype);
    
    /* runtime hack because udpv6_sendmsg is not exported!!! */
    res = sock_create_kern(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, &sock);
    udpv6_prot = sock->sk->sk_prot;
    inet6_mhost_proto.udp_sendmsg = udpv6_prot->sendmsg;
    udpmhost_prot.get_port = udpv6_prot->get_port;
    sock_release(sock);

    /* my table functions here */
    mhost_table_register(&inet_mhost_proto);
    mhost_table_register(&inet6_mhost_proto);
//    test_mhost_init();
//    other_mhost_init();
    
    /* startup routes for demo! 
     * MAKE SURE THIS IS SET ACCURATELY OR 
     * NOTHING WILL WORK!!! 
     */
//    sim_init_1();
    
    rc = 0;
out:
    return rc;
};

/* my hardcoded a-priori knowledge for demo!
 * computer "1" only does AF_TESTPROTO and INET
 * computer "2" only does AF_OTHERPROTO and INET
 * computer "3" does both! 
 * note that i'm using sockaddr_mhost just so that
 * i can enter in a port number here without any problems
 */

/*
int sim_init_1(void)
{    
    struct sockaddr_mhost test;
    struct sockaddr_in inet;
        
    test.sa_family = AF_TESTPROTO;
    test.port = htons(8080);
    test.id_no = 3;
    insert_sockaddr_id((struct sockaddr *) &test, 3);
    
    inet.sin_family = AF_INET;
    inet.sin_port = htons(8080);
    inet.sin_addr.s_addr = ntohl(0xC0A832B5);
    insert_sockaddr_id((struct sockaddr *) &inet, 2);

    return 0;
}

int sim_init_2(void)
{    
    struct sockaddr_mhost test;
    struct sockaddr_in inet;
    
    test.sa_family = AF_OTHERPROTO;
    test.port = htons(8080);
    test.id_no = 3;
    insert_sockaddr_id((struct sockaddr *) &test, 3);
    
    inet.sin_family = AF_INET;
    inet.sin_port = htons(8080);
    inet.sin_addr.s_addr = ntohl(0xC0A83285);
    insert_sockaddr_id((struct sockaddr *) &inet, 1);
    
    return 0;
}

int sim_init_3(void)
{    
    struct sockaddr_mhost test;
    
    test.sa_family = AF_TESTPROTO;
    test.port = htons(8080);
    test.id_no = 1;
    insert_sockaddr_id((struct sockaddr *) &test, 1);
    
    test.sa_family = AF_OTHERPROTO;
    test.id_no = 2;
    insert_sockaddr_id((struct sockaddr *) &test, 2);
    
    return 0;
}
*/

MODULE_LICENSE("GPL");

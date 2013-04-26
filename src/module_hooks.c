#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"
#include "mnet_header.h"

// int sim_init_1(void);
// int sim_init_2(void);
// int sim_init_3(void);
int mhost_init(void);
int currentId = 0;

/*******************************************************************************/
// Variables for opening a character device
static struct class *my_class;
/*
 * Added by MSEVILLA (12-9-2012)
 * 	Function for accessing a character device using ioctl
 *	This function is called using ioctl() from mhost_getaddrinfo.c
 */

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long args) {
        switch(cmd) {
    		struct sockaddr_in inet[sizeof(struct sockaddr_in)];
		struct sockaddr ret[sizeof(struct sockaddr)];
		struct sockaddr * ptr;
                case IOCTL_GETADDRINFO:	
			//printk(KERN_INFO "Called IOCTL_GETADDRINFO\n");
	        	memcpy(&inet, &args, sizeof(struct sockaddr_in));
			insert_sockaddr_id((struct sockaddr *) &inet, currentId);
                        break;
		case IOCTL_FINISHED_LOOKUP:
			//printk(KERN_INFO "Called IOCTL_FINISHED_LOOKUP\n");
			ptr = mhost_get_l3_head(currentId);
			memcpy(&ret, ptr, sizeof(struct sockaddr)); 
			//printk(KERN_INFO "returned port: %d", ntohs(casted_ptr->sin_port));
			//printk(KERN_INFO "returned addr: %d", htonl(casted_ptr->sin_addr.s_addr));
			copy_to_user((struct sockaddr *) args, &ret, sizeof(struct sockaddr));
			currentId++;
			break;
        }

        return 0;
}
/*
 * Any calls to the correct device file (in our case, /dev/my_dev) will be routed 
 * here first. This routes control to the correspong function. These file operations 
 * can be found in linux/fs.h. In this case, if someone calls ioctl(), it will be 
 * be re-routed here, to .unlocked_ioctl.
 */
static struct file_operations fops={
        .unlocked_ioctl = device_ioctl
};

/*******************************************************************************/
int init_module(void)
{
    int retval;
    printk(KERN_INFO "adding mobile_host module\n");
	retval = mhost_init();
    return retval;
}

void cleanup_module(void)
{
	/* 
	 * Added by MSEVILLA (12-9-2012)
	 *  Cleanup my character device
	 */
	device_destroy(my_class, MKDEV(major_no, 0));
	class_unregister(my_class);
	class_destroy(my_class);
	unregister_chrdev(major_no, DEVICE_NAME);
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

    /******************************************************************************* 
     * Added by MSEVILLA (12-9-2012)
     * 	I need to create a character device so that I can write to our LKM 
     * 	from user-space
     */
    /* Registering a device with the kernel
     * character devices aer access through device files in /dev
     * major #: which driver handles which device file
     * minor #: used by the driver iteslf to figure out what device it's operating on
     * I made major # 0 so the kernel dynamically allocates a major # 
    */
    major_no = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_no < 0) {
            printk(KERN_INFO "MSEVILLA ERROR: registering char device failed with %d\n", major_no);
            return major_no;
    }
    printk(KERN_INFO "MSEVILLA: major_num: %d\n", major_no);
    
    /* 
     * Make a device that we can write to
     */
    // Create a struct class structure
    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    // Create a device and register it with sysfs
    device_create(my_class, NULL, MKDEV(major_no, 0), NULL, DEVICE_NAME);

    /******************************************************************************* 
     *******************************************************************************/
   
    rc = 0;
out:
    return rc;
};

MODULE_LICENSE("GPL");

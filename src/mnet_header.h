#include <linux/ioctl.h>
#include <linux/kdev_t.h> /* for MKDEV */

#define DEVICE_NAME "my_dev"
#define DEVICE_PATH "/dev/my_dev"
//#define WRITE 0
static int major_no;
#define MAGIC_NO '4'
/* 
 * Set the message of the device driver 
 */
#define IOCTL_GETADDRINFO _IOR(MAGIC_NO, 0, char *)
#define IOCTL_FINISHED_LOOKUP _IOR(MAGIC_NO, 1, struct sockaddr *)
//#define IOCTL_FINISHED_LOOKUP _IOR(MAGIC_NO, 1, char *)

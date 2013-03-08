#ifndef _KERNEL_INCLUDES_H
#define _KERNEL_INCLUDES_H

#include <net/udp.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#include <net/dst.h>
#include <net/flow.h>
#include <net/route.h>

#include <net/transp_v6.h>

#include <linux/ip.h>
#include <linux/in6.h>
#include <linux/socket.h>
#include <linux/udp.h>
#include <linux/jhash.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/igmp.h>
#include <linux/netdevice.h>

/* 
 * Added by MSEVILLA (12-9-2012)
 * 	to get ioctl functionality
 */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#endif

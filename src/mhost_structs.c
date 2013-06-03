/* THIS file contains all the structs exposed to multiple files
 * (that is, every struct declared in mhost_structs.h)
 */

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"

const struct in6_addr in6_any = IN6ADDR_ANY_INIT;

/* taken from af_mhost.c ======================== */

const struct net_proto_family mhost_family_ops = {
	.family     = AF_MHOST,
	.create     = mhost_create,
    .owner      = THIS_MODULE,
};
EXPORT_SYMBOL(mhost_family_ops);

/*  version of inet_dgram_ops, set as sock->ops */
const struct proto_ops mhost_dgram_ops = {
    .family         = AF_MHOST,
    .owner          = THIS_MODULE,
    .release        = mhost_release,
    .bind           = mhost_bind,
    .connect        = mhost_dgram_connect,
    .socketpair     = sock_no_socketpair,
    .accept         = sock_no_accept,
    .getname        = mhost_getname,
    .poll           = udp_poll,
    .ioctl          = mhost_ioctl,
    .listen         = sock_no_listen,
    .shutdown       = mhost_shutdown,
    .setsockopt     = sock_common_setsockopt,
    .getsockopt     = sock_common_getsockopt,
    .sendmsg        = mhost_sendmsg,
    .recvmsg        = mhost_recvmsg,
    .mmap           = sock_no_mmap,
    .sendpage       = mhost_sendpage,
};
EXPORT_SYMBOL(mhost_dgram_ops);

/* taken from mhost_inet.c ======================== */

/* this entry is for IPv4 compatibility */
struct mhost_proto inet_mhost_proto = {
    .family         = AF_INET,
    .owner          = THIS_MODULE,
    .next           = NULL,
    .udp_sendmsg    = udp_sendmsg,
};

struct mhost_proto inet6_mhost_proto = {
    .family         = AF_INET6,
    .owner          = THIS_MODULE,
    .next           = NULL,
    .udp_sendmsg    = NULL,
};

/* "test" sockaddr_in to 127.0.0.1:8080 */
struct sockaddr_in test_sa = {
    .sin_family = AF_INET,
    .sin_port   = 0x901F,
    .sin_addr   = {0x0100007F},
    .sin_zero   = {0x00},
};

/* "test" sockaddr_in6 to fe80::1:8080 */
struct sockaddr_in6 test6_sa = {
    .sin6_family    = AF_INET6,
    .sin6_port      = 0x901F,
    .sin6_addr      = IN6ADDR_LOOPBACK_INIT,
};

/* taken from udp_mhost.c ======================== */

/* udp_prot */
struct proto udpmhost_prot = {
    .name           = "UDPMHOST",
    .owner          = THIS_MODULE,
    .close          = udp_lib_close,
    .destroy        = udp_destroy_sock,
    .obj_size       = sizeof(struct udp_mhost_sock),
    .slab_flags     = SLAB_DESTROY_BY_RCU,
    .h.udp_table    = &udp_table,
    /* huh? */
    .hash           = udp_lib_hash,
    .unhash         = udp_lib_unhash,
    .memory_allocated  = &udp_memory_allocated,
    .sysctl_mem        = sysctl_udp_mem,
    
    /* custom functions */
    .sendmsg    = udpmhost_sendmsg,
    .recvmsg    = udpmhost_recvmsg,
    .get_port   = NULL,
};
EXPORT_SYMBOL(udpmhost_prot);

struct inet_protosw mhost_dgram_protosw = {
    .type       = SOCK_DGRAM,
    .protocol   = IPPROTO_UDP,
    .prot       = &udpmhost_prot,
    .ops        = &mhost_dgram_ops,
    .no_check   = 0,
    .flags      = 0,
};
struct inet_protosw fake_dgram_protosw = {
    .type       = SOCK_DGRAM,
    .protocol   = IPPROTO_UDP,
    .prot       = &udp_prot,
    .ops        = &inet_dgram_ops,
    .no_check   = UDP_CSUM_DEFAULT,
    .flags      = INET_PROTOSW_PERMANENT,
};
EXPORT_SYMBOL(mhost_dgram_protosw);

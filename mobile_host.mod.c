#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x80567cab, "module_layout" },
	{ 0xa27f6083, "release_sock" },
	{ 0xaa5b0f7, "kmalloc_caches" },
	{ 0xef4cd866, "sock_init_data" },
	{ 0xa2ef34d7, "rps_sock_flow_table" },
	{ 0xafb89178, "sock_queue_rcv_skb" },
	{ 0x4d45d89e, "udp_memory_allocated" },
	{ 0x47c7b0d2, "cpu_number" },
	{ 0x81ef780d, "sk_common_release" },
	{ 0xe9a7394c, "sock_no_sendpage" },
	{ 0x7dbcccc6, "sock_no_mmap" },
	{ 0xd960378, "sock_no_socketpair" },
	{ 0x5eaffe07, "sk_alloc" },
	{ 0xdb01144c, "udp_sendmsg" },
	{ 0xf9d3b04c, "udp_flush_pending_frames" },
	{ 0x50eedeb8, "printk" },
	{ 0x66813d2d, "lock_sock_nested" },
	{ 0xfe601a6d, "__skb_recv_datagram" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xb4390f9a, "mcount" },
	{ 0x3af6e938, "sock_no_listen" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xd335c45d, "skb_push" },
	{ 0x607f96c, "sock_no_accept" },
	{ 0xd3ee1359, "dev_get_by_index" },
	{ 0xbbf4758f, "udp_prot" },
	{ 0xcb2854b2, "lock_sock_fast" },
	{ 0xaeef4f5a, "skb_pull" },
	{ 0x7f3a7f55, "init_net" },
	{ 0x40973662, "sysctl_udp_mem" },
	{ 0xe6f46b3f, "__sock_recv_ts_and_drops" },
	{ 0xc6cbbc89, "capable" },
	{ 0x77f84990, "proto_register" },
	{ 0x96e16041, "inet_dgram_ops" },
	{ 0x56818876, "udp_poll" },
	{ 0x6abb9e80, "inet_release" },
	{ 0x5062db4c, "__alloc_skb" },
	{ 0x572ca142, "udp_lib_get_port" },
	{ 0x6223cafb, "_raw_spin_unlock_bh" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xd0ff5428, "sock_register" },
	{ 0x357e0bd3, "kfree_skb" },
	{ 0xe625c3a3, "inet_sock_destruct" },
	{ 0x7eeeaab7, "skb_copy_datagram_iovec" },
	{ 0x6e712077, "kmem_cache_alloc_trace" },
	{ 0xd3e9386c, "udp_lib_unhash" },
	{ 0xa35de80f, "ipv4_config" },
	{ 0x7eba5dbd, "sock_common_setsockopt" },
	{ 0x37a0cba, "kfree" },
	{ 0xc43ddd9f, "sock_common_getsockopt" },
	{ 0x8fa113bf, "dev_add_pack" },
	{ 0x9a4bb180, "dev_queue_xmit" },
	{ 0x1ed9e8f3, "skb_put" },
	{ 0x6a27bfce, "csum_partial_copy_generic" },
	{ 0xbc27eea8, "udp_table" },
	{ 0xa1a5c890, "skb_free_datagram_locked" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "CEB5C98428928D1485D19F5");

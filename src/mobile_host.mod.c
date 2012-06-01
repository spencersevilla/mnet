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
	{ 0xf6628fc9, "module_layout" },
	{ 0x17e2409c, "release_sock" },
	{ 0x53822150, "kmalloc_caches" },
	{ 0xe33f0f07, "sock_init_data" },
	{ 0xa2ef34d7, "rps_sock_flow_table" },
	{ 0x756cbd57, "sock_queue_rcv_skb" },
	{ 0xa0ff74d6, "udp_memory_allocated" },
	{ 0x47c7b0d2, "cpu_number" },
	{ 0xd2c4b9af, "sk_common_release" },
	{ 0x7c255169, "sock_no_sendpage" },
	{ 0x15af0d03, "sock_no_mmap" },
	{ 0x9e93094c, "sock_no_socketpair" },
	{ 0xde89620a, "sk_alloc" },
	{ 0x59e89bbe, "udp_sendmsg" },
	{ 0xe37ccbe8, "udp_flush_pending_frames" },
	{ 0x27e1a049, "printk" },
	{ 0x1072a394, "csum_partial_copy_from_user" },
	{ 0x2b69e27b, "lock_sock_nested" },
	{ 0xc9f6b040, "__skb_recv_datagram" },
	{ 0xb4390f9a, "mcount" },
	{ 0xf4b31bc9, "sock_no_listen" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xac9ad832, "skb_push" },
	{ 0x861f5828, "sock_no_accept" },
	{ 0xbd4d3c03, "dev_get_by_index" },
	{ 0x9c3bc8ab, "udp_prot" },
	{ 0x2f7c9c62, "lock_sock_fast" },
	{ 0x361dbd01, "skb_pull" },
	{ 0x40973662, "sysctl_udp_mem" },
	{ 0xcb497666, "__sock_recv_ts_and_drops" },
	{ 0xc6cbbc89, "capable" },
	{ 0xfeb2f5d9, "proto_register" },
	{ 0x670b50e5, "inet_dgram_ops" },
	{ 0x70ba528b, "udp_poll" },
	{ 0xce355510, "inet_release" },
	{ 0x77782dca, "__alloc_skb" },
	{ 0xc7715560, "udp_lib_get_port" },
	{ 0xba63339c, "_raw_spin_unlock_bh" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x7d24ad84, "sock_register" },
	{ 0x39e2795, "kfree_skb" },
	{ 0x21682f87, "inet_sock_destruct" },
	{ 0x899559c6, "skb_copy_datagram_iovec" },
	{ 0x4a21112a, "kmem_cache_alloc_trace" },
	{ 0xd1c8a95f, "udp_lib_unhash" },
	{ 0xa35de80f, "ipv4_config" },
	{ 0xfb35013e, "sock_common_setsockopt" },
	{ 0x37a0cba, "kfree" },
	{ 0x604900d0, "sock_common_getsockopt" },
	{ 0x88fed9c4, "dev_add_pack" },
	{ 0xe56a4252, "dev_queue_xmit" },
	{ 0x5caf4af1, "skb_put" },
	{ 0x2e5a704c, "udp_table" },
	{ 0xe3c80cab, "skb_free_datagram_locked" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D05BD566F414DA158BDA8EC");

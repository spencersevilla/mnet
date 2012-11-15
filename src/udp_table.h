#ifndef _udp_table_h
#define _udp_table_h

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"
#include "mhost_funcs.h"

int udp_table_insert(struct sock *sk, int port);
int udp_table_remove(struct sock *sk);
struct sock * udp_table_lookup(int port);

#endif /* _udp_table_h */
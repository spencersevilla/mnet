#ifndef _MHOST_FUNCS_H
#define _MHOST_FUNCS_H

/* This file contains functions that are called outside of the files they're in */

#include "kernel_includes.h"
#include "mhost_defs.h"
#include "mhost_structs.h"

/* defined in mhost_table.c */
int mhost_translate_sa(struct sockaddr *sa, struct sock *sk);
int mhost_table_register(struct mhost_proto *proto);
int insert_sockaddr_id(struct sockaddr *sa, short id);
struct mhost_proto * mhost_proto_for_family(short family);


#endif

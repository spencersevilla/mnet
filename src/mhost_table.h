#ifndef _MHOST_TABLE_H
#define _MHOST_TABLE_H

#include "af_mhost.h"

int mhost_table_register(struct mhost_proto *proto);
int mhost_translate_sa(struct sockaddr *sa, struct sock *sk);
int insert_sockaddr_id(struct sockaddr *sa, short id);
struct mhost_proto * mhost_proto_for_family(short family);
/* NOTE: goal here is to produce a sk_buff structure for use in dev_queue_xmit */

#endif /* _MHOST_TABLE_H */
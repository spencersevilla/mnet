//
//  mhost_downstack.h
//  
//
//  Created by spencer on 5/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _mhost_downstack_h
#define _mhost_downstack_h

#include "kernel_includes.h"

/* downstack */
int mhost_finish_output(struct sk_buff *skb, struct net_device *dev, const void *daddr);

extern char baddr[6];
extern char daddr1[6];
extern char daddr2[6];

#endif

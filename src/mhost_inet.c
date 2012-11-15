//
//  mhost_inet.c
//  
//
//  Created by Spencer Sevilla on 4/8/12.
//  Copyright (c) 2012 UCSC School of Engineering. All rights reserved.
//

#include "mhost_inet.h"

/* NOTE: this entry is for IP-layer compatibility */
struct mhost_proto inet_mhost_proto = {
    .family         = AF_INET,
    .owner          = THIS_MODULE,
    .next           = NULL,
    .udp_sendmsg    = udp_sendmsg,
};

/* sockaddr_in to 127.0.0.1:8080 (google.com) */
struct sockaddr_in test_sa = {
    .sin_family = AF_INET,
    .sin_port   = 0x901F,
    .sin_addr   = {0x0100007F},
    .sin_zero   = {0x00},
};

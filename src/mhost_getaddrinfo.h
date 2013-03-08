#include "mnet_header.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

// Addedfor networking
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>

int mhost_getaddrinfo(char * hostname, struct sockaddr * mnet_result);

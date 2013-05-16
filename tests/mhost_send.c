/* Sample UDP sender */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define AF_MHOST 27
#define AF_ADD 1

struct sockaddr_mhost {
    short           sa_family;
    unsigned short  port;
    unsigned int  id_no;
    char            opaque[8];
};

int main(int argc, char**argv)
{
    int sockfd, retval;
    struct sockaddr_mhost addr;
    struct sockaddr_in6 in6_addr;
    char sendline[] = "hello";
    
    /* info here */
    addr.sa_family = AF_ADD;
    addr.port = htons(8080);
    addr.id_no = 1;

    in6_addr.sin6_family = AF_INET6;
    in6_addr.sin6_port = htons(8080);
    in6_addr.sin6_addr = in6addr_loopback;
    
    sockfd = socket(AF_MHOST, SOCK_DGRAM, 0);
    printf("fd: %d\n", sockfd);
    
    retval = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("sendto AF_ADD returned: %d\n", retval);
        
    retval = close(sockfd);
    
    printf("close returned: %d\n", retval);
    return 0;
}

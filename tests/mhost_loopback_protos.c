/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define AF_MHOST 27
#define AF_TESTPROTO 1
#define AF_OTHERPROTO 3

struct sockaddr_mhost {
    short           sa_family;
    unsigned short  port;
    unsigned short  id_no;
    char            opaque[10];
};

int main(int argc, char**argv)
{
    int sockfd, retval;
    struct sockaddr_mhost sm;
    char sendline[] = "hello111";
    const struct msghdr mh;
    
    sm.id_no = 0;
    sm.port = htons(8080);
    sm.sa_family = AF_TESTPROTO;
    
    sockfd = socket(AF_MHOST, SOCK_DGRAM, 0);
    printf("fd: %d\n", sockfd);
    
    retval = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&sm, sizeof(sm));
    printf("sendto AF_TESTPROTO returned: %d\n", retval);
    
    sm.sa_family = AF_OTHERPROTO;
    retval = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&sm, sizeof(sm));
    printf("sendto AF_OTHERPROTO returned: %d\n", retval);
    
    retval = close(sockfd);
    
    printf("close returned: %d\n", retval);
    return 0;
}

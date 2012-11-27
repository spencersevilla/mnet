/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char**argv)
{
    int sockfd, retval;
    struct sockaddr_in6 servaddr, cliaddr;
    char sendline[] = "hello111";
    const struct msghdr mh;
    
    servaddr.sin6_port = htons(8080);
    servaddr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &servaddr.sin6_addr);

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    printf("fd: %d\n", sockfd);
    
    retval = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("sendto returned: %d\n", retval);
    
    retval = close(sockfd);
    
    printf("close returned: %d\n", retval);
    return 0;
}

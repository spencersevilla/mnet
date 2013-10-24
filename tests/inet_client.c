/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define AF_MHOST 27

int main(int argc, char**argv)
{
    int sockfd, retval;
    struct sockaddr_in servaddr;
    char sendline[] = "hello111";
    
    inet_pton(AF_INET, "8.8.8.7", &servaddr.sin_addr);
    servaddr.sin_port = htons(8080);
    servaddr.sin_family = AF_INET;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("fd: %d\n", sockfd);
    
    retval = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("sendto returned: %d\n", retval);
    
    retval = close(sockfd);
    
    printf("close returned: %d\n", retval);
    return 0;
}

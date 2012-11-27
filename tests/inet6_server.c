/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char**argv)
{
    int sockfd, sockfd2, n, retval;
    struct sockaddr_in6 servaddr,cliaddr;
    socklen_t len;
    char mesg[1000];
    char address[16] = "::1";
    
    sockfd = socket(AF_INET6,SOCK_DGRAM,0);
    if (!sockfd)
	return 0;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
//    inet_pton(AF_INET6, address, &(servaddr.sin6_addr));
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port=htons(8080);
    
    retval = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if (retval)
        return 0;
    
    for (;;)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
        sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        printf("-------------------------------------------------------\n");
        mesg[n] = 0;
        printf("Received the following:\n");
        printf("%s",mesg);
        printf("-------------------------------------------------------\n");
    }
}

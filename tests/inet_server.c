/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char**argv)
{
    int sockfd, sockfd2, n, retval;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char mesg[1000];
    char address[16] = "127.0.0.1";
    char address2[16] = "192.168.1.102";
    
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    sockfd2 = socket(AF_INET,SOCK_DGRAM,0);
    if (!sockfd || !sockfd2)
	return 0;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, address2, &(servaddr.sin_addr));
    servaddr.sin_port=htons(8080);
    
    bzero(&cliaddr,sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    inet_pton(AF_INET, address, &(cliaddr.sin_addr));
    cliaddr.sin_port=htons(8080);
    
    retval = bind(sockfd2,(struct sockaddr *)&cliaddr,sizeof(servaddr));
    if (retval)
        return 0;
    
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

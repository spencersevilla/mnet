/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define AF_MHOST 27

int main(int argc, char**argv)
{
    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char mesg[1000];
    
    sockfd=socket(AF_MHOST,SOCK_DGRAM,0);
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_MHOST;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(8080);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    printf("-------------------------------------------------------\n");
    for (;;)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
//        sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        mesg[n] = 0;
        printf("Received the following:\n");
        printf("%s\n",mesg);
        printf("-------------------------------------------------------\n");
    }
}


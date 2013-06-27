/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/time.h>

#define AF_MHOST 27

int onerun(struct timeval *ret)
{
    int sockfd, retval;
    int size = 1470;
    int i;
    struct sockaddr_in servaddr;

	struct timeval tv1, tv2;
	struct timezone tz1, tz2;
	struct tm *tm1, *tm2;

    /* write gibberish */
    char *content;
    content = malloc(size);
    for (i = 0; i < size; i++) {
    	content[i]=rand()%26+'a';
    }

    /* addr info here */
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(8080);
    servaddr.sin_family = AF_INET;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    /* start timer */
	gettimeofday(&tv1, &tz1);
    // tm1 = localtime(&tv1.tv_sec);
    // printf("START: %d:%02d:%02d %d %d \n", tm1->tm_hour, tm1->tm_min,
    //     tm1->tm_sec, tv1.tv_usec, tv1.tv_sec);

    /* SEND!!! "magic number" = 10gb transfer */
    for(i = 0; i < 730436; i++) {
	    retval = sendto(sockfd, content, size, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }

    /* end timer */
    gettimeofday(&tv2, &tz2);
    // tm2 = localtime(&tv2.tv_sec);
	// printf("END: %d:%02d:%02d %d \n", tm2->tm_hour, tm2->tm_min,
	//     tm2->tm_sec, tv2.tv_usec);

    // printf("START2: %d:%02d:%02d %d %d \n", tm1->tm_hour, tm1->tm_min,
    //     tm1->tm_sec, tv1.tv_usec, tv1.tv_sec);

    /* parse results */
    // printf("START: %d sec %d microsec ", tv1.tv_sec, tv1.tv_usec);
    // printf("FINISH: %d sec %d microsec ", tv2.tv_sec, tv2.tv_usec);

    timersub(&tv2, &tv1, ret);

    close(sockfd);
    return 0;
}

int main(int argc, char**argv)
{
    struct timeval t;
    float s = 0.0;
    int i;
    int count = 4;

    for (i = 0; i < count; i++) {
        onerun(&t);
        printf("DIFFERENCE: %d.%d seconds \n", t.tv_sec, t.tv_usec);
        s += t.tv_sec;
        s += ((t.tv_usec) * 0.000001);
    }

    printf("SUM: %f seconds \n", s);
    printf("AVG: %f seconds \n", s / (float) count);
    return 0;
}
#include "mhost_getaddrinfo.h"
#include "mnet_header.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define SUCCESS 0

int mhost_getaddrinfo(char * hostname, struct sockaddr * mnet_result) {
	int fd, ret, err, counter;
	struct addrinfo hints, *result, *addrpointer;
	struct sockaddr mnet_addr[sizeof(struct sockaddr)];
	struct sockaddr_in casted_addr[sizeof(struct sockaddr_in)];
	struct sockaddr_in * casted_mnet_addr;
	
	char str[INET6_ADDRSTRLEN];
	int args[sizeof(struct sockaddr)];

	printf("Starting mhost_getaddrinfo library\n");
	printf("Written by M. Sevilla, K. Abas, L. Liu\n");
	printf("   {msevilla, kabas, lliu}@soe.ucsc.edu\n\n");
	
	printf("Given hostname: %s\n", hostname);	
	/*
	 * Open the character device
	 * 	We write to this character device, which forwards
	 *	data to the kernel. 
	 * 
	 *	This character device, located at /dev/my_dev was 
	 *	prepared and loaded by module_hooks.c - wihtout
	 * 	this module loaded, this library is not functional
	 */
	fd = open(DEVICE_PATH, O_RDWR);
	if (fd < 0) {
	        printf("ERROR - open failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	/* 
	 * Call getaddrinfo()
	 *	This returns a list of address structures for the
	 *	given hostname.
	 *	
	 *	We must construct hints that getaddrinfo() can demux
	 *	correctly.
	 */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	err = getaddrinfo(hostname, "80", &hints, &result);
	
	if (err != 0) {
	        printf("ERROR - getaddrinfo: %s\n", gai_strerror(err));
	        exit(EXIT_FAILURE);
	}

	/* 
	 * Iterate through the returned addrs and send them into the kernel
	 * 	Each address structure is sent one at a time since ioctl does 
	 *	not allow a ton of bits to flow through at one time.
	 */	
	counter = 0;
	for (addrpointer = result; addrpointer != NULL; addrpointer = addrpointer->ai_next) {
		// Print some stuff out for the user
	        memcpy(&casted_addr, addrpointer->ai_addr, sizeof(struct sockaddr_in));
	        inet_ntop(AF_INET, &casted_addr->sin_addr, str, INET6_ADDRSTRLEN);
		printf("-- Sending address %d (%s)--\n", counter, str);
		printf("family: %d, port: %d, address: %d\n", 
			casted_addr->sin_family, 
			ntohs(casted_addr->sin_port), 
			htonl(casted_addr->sin_addr.s_addr));

		/*
		 *  Use ioctl to send the packets into the kernel
		 *	ioctl can be used just like a system call but  can be loaded and run
		 *	dynamically (no re-compiling the kernel). 
		 *
		 * 	For some reason, I casted these into sockaddr_in, but this isen't 
		 * 	necessary.
		 */
		ret = ioctl(fd, IOCTL_GETADDRINFO, *casted_addr);
		if (ret < 0) {
		        printf("getaddrinfo failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Pause to prevent synchronization errors
		usleep(500000);
	        counter++;
	}
	
	/* 
	 * Use ioctl to print the table and to get the first address structure
	 * 	The first address structure will be stored by the user and used in subsequent
	 * 	connections. This will be known as the mnet_address. The user does not need to
	 *	manage or understand ports and IPs. Hooray.
	 */
	ret = ioctl(fd, IOCTL_FINISHED_LOOKUP, mnet_addr);
	if (ret < 0) {
	        printf("finished-lookup failed: %d\n", ret);
	        printf("errno: %s\n", strerror(errno));
		exit(EXIT_FAILURE);	
	}

	// Print the address of sockaddr structure that the user receives
	casted_mnet_addr = (struct sockaddr_in *) &mnet_addr;
	inet_ntop(AF_INET, &casted_mnet_addr->sin_addr, str, INET6_ADDRSTRLEN);
	printf("\n-- COMPLETED SUCCESSFULLY -- (%s)\n", str);
	printf("* use the returned sockaddr in connections\n");
	printf("family: %d, port: %d, address: %d\n\n", 
		casted_mnet_addr->sin_family, 
		ntohs(casted_mnet_addr->sin_port),
		htonl(casted_mnet_addr->sin_addr.s_addr));
	memcpy(mnet_result, mnet_addr, sizeof(struct sockaddr));
	// Close the character device
	close(fd);
	return SUCCESS;
}

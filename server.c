/*
 * Implementation of RFC-868
 * link: https://tools.ietf.org/html/rfc868
 *
 * Server responds to all incoming requests with a 32-bit integer representing
 * the number of seconds since 00:00 1 January 1900.  
 */ 

#include "server.h"
/* 
 * "Business logic" 
 * Unix epoch 00:00 1 January 1970
 * Unix epoch treats each day as if it consisted of 86400 seconds, the same is done here. 
 * The RFC says that the number of seconds from 00:00 January 1 1900 to 00:00 January 1 1970 is
 * 2,208,988,800 = 70*86400*365 +17*86400. 17 being number of leap years in that duration. 
 *
 * The function time() is used to query the operating system about the number of seconds since unix epoch.
 *
 */
uint32_t getTime() {
    uint32_t t = time(NULL);
    int years = t/(DAY_IN_YEAR*SEC_IN_DAY);
    int leapYears = years / 4;

	return time(NULL)+EPOCH_2_EPOCH;
}



/* 
 * initialize a server. Protocol to use, protocol port and address given as arguments.  
 */
server* createServer(char* protocol, char* address, int port) {
	
	/* declare server struct variable and socket descriptor */
	server *srv;
       	int sockDescriptor;
	
	/* allocate space for server */
	if((srv = (server*) malloc(sizeof(server))) == NULL) {
		fprintf(stderr, "Error allocating memory for server struct.\n");
		exit(EXIT_FAILURE);
	}

	/* get protocol object from string argument */
	if((srv->protocol = getprotobyname(protocol)) == NULL) {
		fprintf(stderr, "Error finding protocol.\n");
		exit(EXIT_FAILURE);
	}
	
	/* protoent has a an integer field p_proto. The integers associated with protocols 
	 * can be found in /etc/protocols TCP is 6, UDP is 17. If TCP use reliable socket 
	 * stream. If UDP use datagrams. 
	 */
	int communicationType;
	/* consider changing "magic numbers" to constants */
	if(srv->protocol->p_proto == 6)
		communicationType = SOCK_STREAM;
	else if(srv->protocol->p_proto == 17)
		communicationType = SOCK_DGRAM;
	else {
		fprintf(stderr, "Error. Server only supports TCP and UDP.");
		exit(EXIT_FAILURE);
	}

	/* use ip */
	srv->localAddr.sin_family = PF_INET;

	/* assign protocol port number */
	srv->localAddr.sin_port = htons(port);
	
	/* assign local address forstår ikke helt s_addr, vist et struct som er sin_addrresse. 
	 * i hvert fald vi bruger loopback adressen. We expect 127.0.0.1
	 */
	srv->localAddr.sin_addr.s_addr = inet_addr(address);
	
	/* size of address struct */
	socklen_t addrlen = sizeof(srv->localAddr);

	/* create socket. Either one for connection oriented communication using TCP or 
	 * connectionless datagram exchange using UDP 
	 */
	if((srv->sock = socket(PF_INET, communicationType, 0)) < 0) {
		fprintf(stderr, "Error creating socket.\n");
		exit(EXIT_FAILURE);
	}

	/* connect socket to local endpoint cast to struct sockaddr 
	 */
	if((bind(srv->sock, (const struct sockaddr*) &srv->localAddr, sizeof(srv->localAddr))) < 0) {
		fprintf(stderr, "Error binding socket...\n");
		exit(EXIT_FAILURE);
	}

	return srv;
}	

/*
 * clean up
 */
void deleteServer(server *srv) {
	close(srv->sock);
	free(srv);
}

/*
 * run TCP server
 */
void runServerTCP(server* srv) {
	
	/* declare some variables */
	int childDescriptor;
	struct sockaddr_in remoteAddr;
	socklen_t size = sizeof(remoteAddr);
	char requestBuffer[MAXLINE];
	char replyBuffer[MAXLINE];
	int pid;
	int status;

	/* listen for connections */	
	if(listen(srv->sock, QSIZE) < 0) {
		fprintf(stderr, "Error listening...\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Time service ready \n");
	while(1) {
		if(childDescriptor = accept(srv->sock, (struct sockaddr*) &remoteAddr, &size) < 0) {
			fprintf(stderr, "Error binding socket...\n");
		}

		/* display sender information */
		printf("Received request from: %s\n", inet_ntoa(remoteAddr.sin_addr));	
		
		if(pid = fork() == 0) { 			
			/* compute reply and convert it to int with sprintf() */
			sprintf(replyBuffer, "%ld", (long) getTime());	

			/* print reply to console */
			printf("Sending %s\n", replyBuffer);

			/* send reply to client. We don not care to read their request */
			send(childDescriptor, replyBuffer, sizeof(replyBuffer), 0);
			
			/* close child descriptor */
			close(childDescriptor);

			/* end child process */
			exit(0);
		}

		/* close newly created file descriptor in parent */
		close(childDescriptor);	

	}

	/* close file descriptor and free allocated memory. We shouldn't reach this point. */
	deleteServer(srv);

}


/*
 * The UDP server does not listen for connections and accept connections like the TCP server. Instead it receives a datagram
 * from someone and replies with a datagram.  
 *
 * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
 *                 struct sockaddr *src_addr, socklen_t *addrlen);
 *
 *
 * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
 *
 * This server does not fork a child process to handle requests
 */
void runServerUDP(server* srv) {
	/* declare some variables */
	int childDescriptor;
	struct sockaddr_in remoteAddr;
	socklen_t size = sizeof(remoteAddr);
	char requestBuffer[MAXLINE];
	char replyBuffer[MAXLINE];
	int pid;
	int status;

	/* display message on console */
	printf("UDP time service ready\n");
	
	while(1) {
		/* receive datagram */
		int msgSZ = recvfrom(srv->sock, requestBuffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &remoteAddr, &size);
		
		/* display sender information */
		printf("Received request from: %s\n", inet_ntoa(remoteAddr.sin_addr));	
	
		/* compute reply and convert it to int with sprintf() */
		//sprintf(replyBuffer, "%x", (uint32_t) getTime());	
		uint32_t reply = htonl(getTime());
		/* print reply to console */
		printf("Sending %u\n", reply);

		/* respond */
		msgSZ = sendto(srv->sock, &reply, sizeof(reply), MSG_CONFIRM, (struct sockaddr*) &remoteAddr, size);
	}

	/* control does not reach this point */
	deleteServer(srv);
}

/* display instructions on command line argument */
void usage(char* arg) {
	printf("Usage: %s <port>\n", arg);
	exit(1);
}


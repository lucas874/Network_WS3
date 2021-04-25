
#include <stdio.h>
#include <unistd.h> /* wrappers for system calls */ 
#include <stdlib.h>
#include <getopt.h>  /* parse command line arguments */
#include <stdarg.h> /* allows same function to accept varying number of arguments like printf() */
#include <sys/types.h> /* size_t, clock_t, time_t */ 
#include <errno.h> /* error codes */
#include <fcntl.h> /* manipulate file descritor */ 
#include <time.h> /* time. time_t time(time_t* seconds). Returns time in seconds since 00:00 1 January 1970. If seconds is NULL returns integer, else stores result in argument */
#include <sys/socket.h>  /* create endpoint for network communication */
#include <netdb.h> /* gethostbyname(namestr), gethostbyaddr(adr, len, type) */
#include <netinet/in.h> /* sockaddr_in */ 
#include <arpa/inet.h> /* inet_aton() without complaints from compiler */
#include <string.h>

#define MAXLINE 1024
#define YEAR 31536000
#define DAY 86400


/* display instructions on command line argument */
void usage(char* arg) {
	printf("Usage: %s <serverhost> <port>\n", arg);
	exit(1);
}

int main(int argc, char* argv[]) {

	if(argc < 3) 
		usage(argv[0]);


	int descriptor;
	/* create socket, reliable sock_det stream so tcp */
	if((descriptor = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error creating sock_det...\n");
		exit(EXIT_FAILURE);
	}

	/* set up object for target address */
	struct sockaddr_in remoteAddr;
	remoteAddr.sin_port = htons(atoi(argv[2]));
	remoteAddr.sin_family = PF_INET;	
	inet_aton(argv[1], &remoteAddr.sin_addr);
	
	/* establish connection */
	connect(descriptor, (const struct sockaddr*) &remoteAddr, sizeof(remoteAddr));
	
	/* send request */
	char* buffer = "hej\0";
	send(descriptor, buffer, sizeof(buffer), 0);
	
	/* receive reply */
	char buffer_reply[MAXLINE];		
	recv(descriptor, buffer_reply, MAXLINE*sizeof(buffer_reply), 0);

	/* display reply */
	printf("%s\n", buffer_reply);	

	return 0;
}

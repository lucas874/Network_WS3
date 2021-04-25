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
#include <netinet/in.h> /* sock_daddr_in */ 
#include <arpa/inet.h> /* inet_aton() without complaints from compiler */
#include <sys/wait.h> /* waitpid */
#include <stdint.h>

#define YEAR 31536000
#define DAY 86400
#define MAXLINE 1024


/* display instructions on command line argument */
void usage(char* arg) {
	printf("Usage: %s <serverhost> <port>\n", arg);
	exit(1);
}

int main(int argc, char* argv[]) {

	if(argc < 3) 
		usage(argv[0]);

	int descriptor;
	/* create sock_det, reliable sock_det stream so tcp */
	if((descriptor = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "Error creating sock_det...\n");
		exit(EXIT_FAILURE);
	}
	/* set up address object  */
	struct sockaddr_in remoteAddr;
	remoteAddr.sin_port = htons(atoi(argv[2]));
	remoteAddr.sin_family = PF_INET;	
	inet_aton(argv[1], &remoteAddr.sin_addr);

	char buffer[MAXLINE];	
	char replyBuffer[MAXLINE];		
	socklen_t size = sizeof(remoteAddr);
    	char *ptr; // used for strtol
	/* send request */
	int msgSZ = sendto(descriptor, (const char*) buffer, MAXLINE, MSG_CONFIRM, (struct sockaddr*) &remoteAddr, size);
		
	/* receive reply */
	uint32_t reply;
	msgSZ = recvfrom(descriptor, &reply, 9, MSG_WAITALL, (struct sockaddr*) &remoteAddr, &size);


	/* display reply */
	reply = ntohl(reply);
	printf("%u\n", reply);
	long val;
	//val = ntohl(replyBuffer);
	//sprintf(replyBuffer, "%u", replyBuffer);
	
    	val = (long) reply;
    	struct tm* readable_time;
    	readable_time = localtime(&val);
    	//printf("%u\n\n", val); 
    /* tm_mon starts at 0!!! */
    printf("Aarhus : %02d/%02d/%4d  %02d:%02d:%02d\n", readable_time->tm_mday, readable_time->tm_mon+1, 1900+readable_time->tm_year-70, 
            readable_time->tm_hour, readable_time->tm_min, readable_time->tm_sec);
    
    return 0;
}


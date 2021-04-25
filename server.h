/*
 * Implementation of RFC-868
 * link: https://tools.ietf.org/html/rfc868
 *
 * Server responds to all incoming requests with a 32-bit integer representing
 * the number of seconds since 00:00 1 January 1900.  
 */ 

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

#define EPOCH_2_EPOCH 2208988800 /* seconds between 1 Jan 1900 00:00 to 1 Jan 1970 00:00 */
#define PORT 50000 // not used remove
#define QSIZE 10 /* size of queue for requests in TCP server */
#define MAXLINE 1024 /* size of buffers passed around */
#define SEC_IN_DAY 86400
#define DAY_IN_YEAR 365
#define SEC_IN_LEAP 31622400
#define SEC_IN_NONLEAP 31536000

typedef int64_t timestamp_t;
int32_t sprintf_timestampAsYYYYMMDDHHMMSS ( char* buf, timestamp_t timestamp );

/* compute content of reply message */
time_t getTime();

/*
 * define server type 
 */
typedef struct {
	int sock; /* socket file descriptor */
	int protoPort; /* port to use */
	struct protoent *protocol; /* specify protocol to use TCP or UDP */
	struct sockaddr_in localAddr; /* struct used to store address, port, protocol family */
} server;

server* createServer(char* protocol, char* address, int port);

void deleteServer(server* srv);

void runServerTCP(server* srv);

void runServerUDP(server* srv);

void usage(char* arg);

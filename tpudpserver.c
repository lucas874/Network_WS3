#include "server.h"

/* create and run UDP time server */
int main(int argc, char* argv[]) {
	/* display instructions on how to run. argv[0] is name of executable */
	if(argc < 2)
		usage(argv[0]);	

	/* create server using given arguments. protocol argument is "hardcoded" */
	server* srv = createServer("udp", "127.0.0.1", atoi(argv[1]));

	/* run server */
	runServerUDP(srv);	
}

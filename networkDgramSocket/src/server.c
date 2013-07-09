#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    // prevent SIGPIPE
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGPIPE, &sa, 0 );

    int sockfd, rval;
    struct sockaddr_in server, client;

    char buf[BUF_MAX];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        ERROR("opening stream socket");
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, 
	    sizeof(yes))) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SOCK_SERV_PORT);

    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server))) {
        ERROR("binding stream socket");
    }
    listen(sockfd, 5); // 5 is the max connections
    int clilen = sizeof(client);
    static int record = 0;
    
    for (;;) {
	memset(buf, 0, sizeof(char) * BUF_MAX);
	rval = read(sockfd, buf, BUF_MAX - 1);
	record_time();
	LOG("====");
	if (rval < 0) {
	    LOG("ERROR(%d): writing on stream socket error %d", rval, record++);
	    perror("write stream message");
	} else if (rval == 0) {
	    LOG("Ending connection");
	    break;
	}
	LOG("((%s))", buf);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

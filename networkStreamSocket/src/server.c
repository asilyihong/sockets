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

    int sockfd, client_fd, rval;
    struct sockaddr_in server, client;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    int msg_len = strlen(DATA_MSG), clilen = sizeof(client);
    static int record = 0;
    
    for (;;) {
	client_fd = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&clilen);
	if (client_fd < 0)
	    continue;
	LOG("get client fd(%d)", client_fd);
	for (;;) {
	    LOG("====");
	    record_time();
	    rval = write(client_fd, DATA_MSG, msg_len);
	    if (rval < 0) {
		LOG("ERROR(%d): writing on stream socket error %d", rval, record++);
		perror("write stream message");
	    } else if (rval == 0) {
		LOG("Ending connection");
		break;
	    }

	    usleep(100000);
	}
	close(client_fd);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

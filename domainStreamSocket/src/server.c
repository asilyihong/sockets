#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    // prevent SIGPIPE
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGPIPE, &sa, 0 );

    int sockfd, client_fd, rval;
    struct sockaddr_un server, client;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ERROR("opening stream socket");
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, 
	    sizeof(yes))) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }

//    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes, 
//		sizeof(yes)) == -1) {
//	LOG("set broadcast failure");
//	ERROR("set broadcast failure");
//    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCK_FILEPATH_NAME);
    unlink(SOCK_FILEPATH_NAME);
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
	    if ((rval = write(client_fd, DATA_MSG, msg_len)) < 0) {
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

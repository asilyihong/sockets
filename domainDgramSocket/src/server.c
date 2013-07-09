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

    int sockfd, rval;
    struct sockaddr_un server, client;

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        ERROR("opening stream socket");
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, 
	    sizeof(yes))) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes, 
		sizeof(yes)) == -1) {
	LOG("set broadcast failure");
	ERROR("set broadcast failure");
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCK_FILEPATH_NAME);
    unlink(SOCK_FILEPATH_NAME);
    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server))) {
        ERROR("binding stream socket");
    }
//    listen(sockfd, 5); // 5 is the max connections
    memset(&client, 0, sizeof(client));
    client.sun_family = AF_UNIX;
    strcpy(client.sun_path, CLIENT_ENDPOINT);
    int msg_len = strlen(DATA_MSG);
    
    for (;;) {
	LOG("====");
	record_time();
	rval = sendto(sockfd, DATA_MSG, msg_len, 0, (struct sockaddr *)&client, sizeof(client));
	if (rval < 0) {
	    perror("write stream message");
	} else if (rval == 0) {
	    LOG("Ending connection");
	    break;
	}

	usleep(100000);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

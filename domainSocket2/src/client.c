#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

#define MAX_PATH_LEN 104

int main(int argc, char *argv[]) {
    // prevent SIGIO
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGIO, &sa, 0 );

    int sockfd, rval;
    struct sockaddr_un serv_addr;
    char buf[BUF_MAX];
    
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
	LOG("opening stream socket");
	ERROR("opening stream socket");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCK_FILEPATH_NAME, MAX_PATH_LEN);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	LOG("connecting stream socket");
	close(sockfd);
	ERROR("connecting stream socket");
    }

    long count = 0;
    for (;;) {
	memset(buf, 0, sizeof(char) * BUF_MAX);
	rval = read(sockfd, buf, BUF_MAX - 1);
	record_time();
	if (rval < 0) {
	    LOG("ERROR reading from socket");
	    perror("ERROR reading from socket");
	    continue;
	} else if (rval == 0){
	    LOG("connection error?");
	}
	LOG("%10ld ((%s))", count++, buf);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

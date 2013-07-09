#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

#define MAX_PATH_LEN 104

int main(int argc, char *argv[]) {
    int sockfd, rval;
    struct sockaddr_un serv_addr, client_addr;
    char buf[BUF_MAX];
    
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
	LOG("opening stream socket");
	ERROR("opening stream socket");
    }
    int fd_flag, result;
    result = fcntl(sockfd, F_GETFD, &fd_flag);
    LOG("get Flag((%d)), result((%d))", fd_flag, result);
    fd_flag |= O_NONBLOCK;
    result = fcntl(sockfd, F_SETFD, fd_flag);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCK_FILEPATH_NAME, MAX_PATH_LEN);
	
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_ENDPOINT, MAX_PATH_LEN);

//    LOG("client path(%s)", client_addr.sun_path);
    unlink(client_addr.sun_path);
    if (bind(sockfd, (struct sockaddr *) &client_addr, sizeof(client_addr))) {
        ERROR("binding stream socket");
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	LOG("connecting stream socket");
	close(sockfd);
	ERROR("connecting stream socket");
    }

    for (;;) {
	memset(buf, 0, sizeof(char) * BUF_MAX);
	rval = read(sockfd, buf, BUF_MAX - 1);
	record_time();
	if (rval < 0) {
	    LOG("ERROR reading from socket");
	    perror("ERROR reading from socket");
	} else if (rval == 0){
	    LOG("connection error?");
	}

	LOG("((%s))", buf);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

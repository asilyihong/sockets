#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

#define MAX_PATH_LEN 104

int main(int argc, char *argv[]) {
    int sockfd, rval;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buf[BUF_MAX];
    char *hostname = SOCK_FILEPATH_NAME;
    
    server = gethostbyname(hostname);
    if (server == NULL) {
	fprintf(stderr, "ERRRO, no such host\n");
	exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
	LOG("opening stream socket");
	ERROR("opening stream socket");
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, 
	   (char *)server->h_addr, 
	   server->h_length);
    serv_addr.sin_port = htons(SOCK_SERV_PORT);

//    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
//	LOG("connecting stream socket");
//	close(sockfd);
//	ERROR("connecting stream socket");
//    }

    int serv_len = sizeof(serv_addr);
    for (;;) {
	memset(buf, 0, sizeof(char) * BUF_MAX);
//	rval = read(sockfd, buf, BUF_MAX - 1);
	rval = recvfrom(sockfd, buf, BUF_MAX - 1, 0, (struct sockaddr *)&serv_addr, (socklen_t *)&serv_len);
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

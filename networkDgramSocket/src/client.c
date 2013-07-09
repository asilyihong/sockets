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
    char *hostname = SOCK_FILEPATH_NAME;
    
    server = gethostbyname(hostname);
    if (server == NULL) {
	fprintf(stderr, "ERRRO, no such host\n");
	exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    int msg_len = strlen(DATA_MSG);

    for (;;) {
	LOG("====");
	record_time();
//	rval = write(sockfd, DATA_MSG, msg_len);
	rval = sendto(sockfd, DATA_MSG, msg_len, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (rval < 0) {
	    LOG("ERROR reading from socket");
	    perror("ERROR reading from socket");
	} else if (rval == 0){
	    LOG("connection error?");
	}

    }
    close(sockfd);
    return EXIT_SUCCESS;
}

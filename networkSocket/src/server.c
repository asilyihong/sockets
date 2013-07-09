/* A simple server in the internet domain using TCP
 *    The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include "utils.h"

#define RETURN_MSG "I got your message"
#define DEFAULT_SIZE 0x8000 // 32 * 1024(32K)
#define MAX_CONNECTION 10;

int main(int argc, char *argv[])
{
    int sockfd, maxfdp;
    char buffer[BUF_MAX];
    struct sockaddr_in serv_addr;
    fd_set fds;
    struct timeval timeout = {0, 500}; // 500 microseconds
    if (argc < 2) {
	fprintf(stderr,"ERROR, no port provided\n");
	exit(1);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
	ERROR("ERROR opening socket");

    maxfdp = sockfd + 1;
    int sockopt_rev, buffer_size, reuse = 1;
    if ((sockopt_rev = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
	    (void *)&reuse, sizeof(reuse))) == -1) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }
    buffer_size = DEFAULT_SIZE;
    sockopt_rev = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, 
	    (char*)&buffer_size, sizeof(buffer_size));
    buffer_size = DEFAULT_SIZE;
    sockopt_rev = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, 
	    (char*)&buffer_size, sizeof(buffer_size));
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
	     sizeof(serv_addr)) < 0) {
	ERROR("ERROR on binding");
    }
    // infinite loop for don't know how much data will be sent from client
    for(;;) {
	head_of_for:
	// read data form client
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	switch(select(maxfdp, &fds, NULL, NULL, &timeout)) {
	case -1: 
	    LOG("get error when use select.");
	    goto head_of_for;
	    break;
	case 0:
	    // LOG("time out or nothing could be read");
	    break;
	default:
	    if (FD_ISSET(sockfd, &fds)) {
		memset(buffer, 0, sizeof(char) * BUF_MAX);
		recv(sockfd, buffer, BUF_MAX -1, MSG_DONTWAIT);
		LOG("Here is the message: ((%s))",buffer);
	    }
	}

//	    // send data to client
//	    memset(rbuf, 0, sizeof(char) * BUF_MAX);
//	    sprintf(rbuf, RETURN_MSG "((%s))", buffer);
//	    if (write(sockfd, rbuf, strlen(rbuf)) < 0)
//		ERROR("ERROR writing to socket");
    }
    close(sockfd);
    return EXIT_SUCCESS; 
}

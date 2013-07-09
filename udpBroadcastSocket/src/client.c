#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "utils.h"


#define DATA "Half a league . . ."
#define MAX_COUNT 10000

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in bc_addr;
    struct hostent *bc_he;

    char buffer[BUF_MAX];
    // get broadcast host name
    bc_he = gethostbyname(BC_ADDR);
    if (bc_he == NULL) {
	fprintf(stderr,"ERROR, no such host\n");
	exit(0);
    }

    memset((char *) &bc_addr, 0, sizeof(bc_addr));
    bc_addr.sin_family = AF_INET;
    bc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bc_addr.sin_port = htons(SERVER_PORT);
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
	ERROR("ERROR opening socket");
    int fd_flag, result;
    result = fcntl(sockfd, F_GETFD, &fd_flag);
    fd_flag |= O_NONBLOCK;
    result = fcntl(sockfd, F_SETFD, fd_flag);
    
    int yes = 1;
    // set socket option
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, 
	    sizeof(yes)) == -1) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&yes, 
	    sizeof(yes)) == -1) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }

    // connect to server
    if (bind(sockfd, (struct sockaddr *)&bc_addr, sizeof(bc_addr))) {
	LOG("bind error");
	ERROR("ERROR bind");
    }

//    int serv_len = sizeof(bc_addr);
    for (;;) {
	// read data from server
	memset(buffer, 0, sizeof(char) * BUF_MAX);
//	if (read(sockfd, buffer, BUF_MAX - 1) < 0) {
	if (recv(sockfd, buffer, BUF_MAX - 1, 0) < 0) {
//	if (recvfrom(sockfd, buffer, BUF_MAX - 1, 0, (struct sockaddr *)&bc_addr, (socklen_t *)&serv_len) < 0) {
	    LOG("ERROR reading from socket");
	    perror("ERROR reading from socket");
	    continue;
	}
	record_time();
	LOG("get from server(%s)",buffer);
    }

    // close connection
    close(sockfd);
    return EXIT_SUCCESS;
}

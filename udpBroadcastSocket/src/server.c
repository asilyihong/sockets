/* A simple server in the internet domain using TCP
 *    The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>

#include "utils.h"

#define RETURN_MSG "I got your message"

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr, bc_addr;
    struct hostent *serv_he, *bc_he;

    if ((serv_he = gethostbyname("127.0.0.1")) == NULL) {
	ERROR("ERROR when get host by name");
    }

    if ((bc_he = gethostbyname(BC_ADDR)) == NULL) {
	ERROR("ERROR when get broadcast host by name");
    }

    sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) 
	ERROR("ERROR opening socket");

    int yes = 1;
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

    // prepare for broadcast side
    memset((char *) &bc_addr, 0, sizeof(serv_addr));
    bc_addr.sin_family = AF_INET;
    bc_addr.sin_port = htons(SERVER_PORT);
    memcpy((char *)&bc_addr.sin_addr.s_addr,
	   (char *)bc_he->h_addr, 
	   bc_he->h_length);

    // prepare for server
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    memcpy((char *)&serv_addr.sin_addr.s_addr,
	   (char *)serv_he->h_addr, 
	   serv_he->h_length);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
	     sizeof(serv_addr)) < 0) {
	ERROR("ERROR on binding");
    }
    
    srand(time(NULL));
    int rand_time;
    for(;;) {
	// send data to client
	record_time();
	if ((sendto(sockfd, RETURN_MSG, sizeof(RETURN_MSG), 0, (struct sockaddr *)&bc_addr, sizeof(bc_addr))) < 0) {
	    LOG("ERROR?");
	    perror("ERROR on sendto");
	}
	record_time();
	LOG("======");
	rand_time = (rand() % 1000) * 1000;
	usleep(rand_time);
    }
    close(sockfd);
    return EXIT_SUCCESS; 
}

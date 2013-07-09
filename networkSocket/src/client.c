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
    int sockfd, val;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUF_MAX];
    if (argc < 3) {
	fprintf(stderr,"usage %s hostname port\n", argv[0]);
	exit(0);
    }
    // get server name
    server = gethostbyname(argv[1]);
    if (server == NULL) {
	fprintf(stderr,"ERROR, no such host\n");
	exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,
	   (char *)server->h_addr, 
	   server->h_length);
//    bcopy((char *)server->h_addr,
//	  (char *)&serv_addr.sin_addr.s_addr,
//	  server->h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) 
	ERROR("ERROR opening socket");
    int fd_flag, result;
    result = fcntl(sockfd, F_GETFD, &fd_flag);
    LOG("get Flag((%d)), result((%d))", fd_flag, result);
    fd_flag |= O_NONBLOCK;
    result = fcntl(sockfd, F_SETFD, fd_flag);
    LOG("set Flag((%d)), result((%d))", fd_flag, result);
//    result = fcntl(sockfd, F_GETFD, &fd_flag);
//    LOG("get Flag((%d)), result((%d))", fd_flag, result);

    // connect to server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
	LOG("connection error");
	ERROR("ERROR connecting");
    }


    int i, count;
    if (argc == 4)
	count = atoi(argv[3]);
    else
	count = MAX_COUNT;
    record_time();
    for (i = 0; i < count; i++) {
	// send data to server
	memset(buffer, 0, sizeof(char) * BUF_MAX);
	sprintf(buffer, DATA " %d", i);
	LOG("%s", buffer);
	if ((val = write(sockfd,buffer,strlen(buffer))) < 0) {
	    LOG("ERROR(%d): writing to socket", val);
	    //ERROR("ERROR writing to socket");
//	} else {
//	    LOG("!write(sockfd,buffer,strlen(buffer)) < 0");
	}
	LOG("write(%d)", val);

	// read data from server
//	memset(buffer, 0, sizeof(char) * BUF_MAX);
//	if (read(sockfd, buffer, BUF_MAX - 1) < 0) {
//	    LOG("ERROR reading from socket");
//	    ERROR("ERROR reading from socket");
////	} else {
////	    LOG("!read(sockfd, buffer, BUF_MAX - 1) < 0");
//	}
//	LOG("%s",buffer);
    }
    record_time();

    // close connection
    close(sockfd);
    return EXIT_SUCCESS;
}

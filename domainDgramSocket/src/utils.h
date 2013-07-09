#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>

#define SOCK_FILEPATH_NAME "/tmp/socket"
#define BUF_MAX 1024
#define DATA_MSG "trans data from server"
#define CLIENT_ENDPOINT "client"

#define ERROR(msg) \
    do { \
	perror(msg); \
	exit(EXIT_FAILURE); \
    } while(0)

#define LOG(x,a...) \
    { \
	printf("\033[1;34m%s \033[32m%s\033[0m(\033[1;36m%d\033[0m) " x "\n", __FILE__, __func__, __LINE__, ##a); \
    }


void record_time();

#endif // __UTILS_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

typedef struct YTNode {
    int clientfd;
    struct sockaddr_un *cli_addr;
    struct YTNode *next, *prev;
} YtLinkedList;
typedef YtLinkedList YtNode;

YtLinkedList *remove_node(YtLinkedList *list, YtNode *node);
YtLinkedList *push_node(YtLinkedList *list, int clientfd, struct sockaddr_un *cli_addr);
static YtNode *YtNode_new();
static void YtNode_free(YtNode *node);

YtLinkedList *gClientList;
pthread_mutex_t gThreadLock;

void *socket_accept_in_thread(void *sockfd) {
    int *fd = (int *)sockfd;

    struct sockaddr_un cli_addr;
    int clientfd, clilen;

    for (;;) {
	clientfd = accept(*fd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	if (clientfd < 0) {
	    continue;
	}

	pthread_mutex_lock(&gThreadLock);
	gClientList = push_node(gClientList, clientfd, &cli_addr);
	pthread_mutex_unlock(&gThreadLock);
    }
}

int main(int argc, char *argv[])
{
    int sockfd, rval;
    struct sockaddr_un serv_addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ERROR("opening stream socket");
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, 
	    sizeof(yes))) {
	LOG("ERROR on set sock opt reuse addr");
	ERROR("ERROR on set sock opt reuse addr");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_FILEPATH_NAME);
    unlink(SOCK_FILEPATH_NAME);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        ERROR("binding stream socket");
    }
    LOG("Socket has name %s", serv_addr.sun_path);
    listen(sockfd, 5); // 5 is the max connections
    int msg_len = strlen(DATA_MSG);
    YtLinkedList *list_iter;

    pthread_mutex_init(&gThreadLock, NULL);
    pthread_t id;
    pthread_create(&id, NULL, (void *)socket_accept_in_thread,(void *) &sockfd);

    for (;;) {
	// need use select to check?
	LOG("======");
	record_time();
	list_iter = gClientList;
	pthread_mutex_lock(&gThreadLock);
	while(list_iter) {
	    rval = sendto(sockfd, DATA_MSG, msg_len, 0, (struct sockaddr *)list_iter->cli_addr, sizeof(struct sockaddr_un));
	    if (rval < 0) {
		LOG("ERROR(%d): writing on stream socket error", rval);
		perror("write stream message");
	    } else if (rval == 0) {
		// TODO; remove from list?
		LOG("Ending connection");
	    }
	    list_iter = list_iter->next;
	}
	pthread_mutex_unlock(&gThreadLock);

	usleep(100000);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

static YtNode *YtNode_new() {
    YtNode *node = malloc(sizeof(YtNode));
    if (!node) { // malloc failure
	return NULL;
    }
    memset(node, 0, sizeof(YtNode));
    node->cli_addr = malloc(sizeof(struct sockaddr_un));
    if (!node->cli_addr) {
	free(node);
	return NULL;
    }
    memset(node->cli_addr, 0, sizeof(struct sockaddr_un));
    return node;
}

static void YtNode_free(YtNode *node) {
    if (!node)
	return;

    free(node->cli_addr);
    free(node);
}

YtLinkedList *remove_node(YtLinkedList *list, YtLinkedList *node) {
    if (!list || !node) // one of list and node is null
	return node;
    YtLinkedList *tmp = NULL, *iter = list;
    while(iter) {
	if (iter == node) {
	    tmp = iter;
	    break;
	}
	iter = iter->next;
    }
    if (!tmp) // cannot find?
	return node;

    if (tmp->next) {
	iter->next->prev = tmp->prev;
    }
    if (tmp->prev) {
	iter->prev->next = tmp->next;
    }
    node->prev = NULL;
    node->next = NULL;

    return node;
}

YtLinkedList *push_node(YtLinkedList *list, int clientfd, struct sockaddr_un *cli_addr) {
    YtLinkedList *iter = list;
    while (iter) {
	if (iter->clientfd == clientfd) { // has pushed to list
	    return list;
	}
	iter = iter->next;
    }

    YtNode *node = YtNode_new();
    if (!node)
	return list;
    
    node->clientfd = clientfd;
    node->cli_addr->sun_family = cli_addr->sun_family;
    strncpy(node->cli_addr->sun_path, cli_addr->sun_path, 108);
    
    node->next = list;
    if (list)
	list->prev = node;

    list = node;
    return list;
}

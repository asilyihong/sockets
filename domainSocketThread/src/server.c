#include <pthread.h>
#include <signal.h>
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

static void *socket_accept_in_thread(void *sockfd);
YtLinkedList *remove_node(YtLinkedList *list, YtNode *node);
YtLinkedList *push_node(YtLinkedList *list, int clientfd, struct sockaddr_un *cli_addr);
static YtNode *YtNode_new();
static void YtNode_free(YtNode *node);

YtLinkedList *gClientList;
pthread_mutex_t gThreadLock;
int list_count = 0;

int main(int argc, char *argv[])
{
    // prevent SIGPIPE
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGPIPE, &sa, 0 );

    int sockfd;
    struct sockaddr_un server;

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

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCK_FILEPATH_NAME);
    unlink(SOCK_FILEPATH_NAME);
    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server))) {
        ERROR("binding stream socket");
    }
    listen(sockfd, 5); // 5 is the max connections

    pthread_mutex_init(&gThreadLock, NULL);
    pthread_t id;
    pthread_create(&id, NULL, (void *)socket_accept_in_thread,(void *) &sockfd);
 
    struct sockaddr_un cli_addr;
    int clientfd, clilen;
    for (;;) {
	clientfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	if (clientfd < 0) {
	    continue;
	}

	pthread_mutex_lock(&gThreadLock);
	gClientList = push_node(gClientList, clientfd, &cli_addr);
	pthread_mutex_unlock(&gThreadLock);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}

static void *socket_accept_in_thread(void *data) {
    int rval;
    YtNode *list_iter, *tmp = NULL;

    LOG("Thread start");
    int msg_len = strlen(DATA_MSG);
    for (;;) {
	LOG("==== %p", gClientList);
	record_time();
	list_iter = gClientList;
	pthread_mutex_lock(&gThreadLock);

	while(list_iter) {
	    rval = write(list_iter->clientfd, DATA_MSG, msg_len);
	    if (rval < 0) {
		gClientList = remove_node(gClientList, list_iter);
		tmp = list_iter;
		list_iter = tmp->next;
		YtNode_free(tmp);
		tmp = NULL;
		LOG("ERROR(%d): writing on stream socket error", rval);
		// end of connection
		perror("write stream message");
		continue;
	    } else if (rval == 0) {
		LOG("Ending connection");
	    }

	    list_iter = list_iter->next;
	}
	pthread_mutex_unlock(&gThreadLock);

	usleep(100000);
    }
    return NULL;
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
	return list;
    YtLinkedList *tmp = NULL, *iter = list;
    while(iter) {
	if (iter == node) {
	    tmp = iter;
	    break;
	}
	iter = iter->next;
    }
    if (!iter) // cannot find?
	return list;

    tmp = iter->next;
    if (tmp)
	tmp->prev = iter->prev;
    tmp = iter->prev;
    if (tmp)
	tmp->next = iter->next;
    
    if (list == node)
	list = list->next;
    return list;
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
    
    list_count++;
    return node;
}

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "serve_static.h"
#include "sysstatd.h"
#include "system_info.h"
#include "artificial_loading.h"
#include "sockets.h"
#include "http_request.h"
#include "http_response.h"

#define ACCEPT_THREADS_MAX 3
#define CONNECTION_THREADS 8
#define MAX_EVENTS 100

static void print_usage(){
	fprintf(stderr, "Usage: sysstatd -p [PORT NUMBER] -R [STATIC PATH]\n");
	fprintf(stderr, "    -p    Specify the port to host the server on.\n");
	fprintf(stderr, 
			"    -R    Specifiy the directory to host static files from\n");
}

char * fpath = NULL;
char * port = NULL;
bool logging = true;

void * connection_handler(void * data){
	int epollfd = (intptr_t) data;
	struct epoll_event events[MAX_EVENTS];
	while(1){
		int n = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		for(int i = 0; i < n; ++i){
			struct conn_state * state = events[i].data.ptr;
			if((events[i].events & !EPOLLIN) || read_request(state)){
				epoll_ctl(epollfd, EPOLL_CTL_DEL, state->fd, &events[i]);
				//if(logging) printf("Connection closed: %d\n", state->fd);
				close(state->fd);
				free(state);
			}
		}
	}
	return NULL;
}

void * accept_routine(void * fd){
	int listenfd = (intptr_t) fd;
	int connfd;
	int epollfd[CONNECTION_THREADS];
	for(int i = 0; i < CONNECTION_THREADS; ++i){
		epollfd[i] = epoll_create(100);
		if(epollfd[i] < 0){
			perror("epoll");
			continue;
		}
		pthread_t thread;
		if(pthread_create(&thread, NULL, &connection_handler,
				(void*)(intptr_t) epollfd[i])){
			fprintf(stderr, "Thread spawn error (Handling thread)\n");
		}
	}
	struct sockaddr_storage clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);
	int j = 0;
	while(1){
		connfd = accept(listenfd, (struct sockaddr *) &clientaddr, 
				&clientaddr_len);
		if(connfd > 0){
			//Set nonblocking
			int flags;
			if(-1 == (flags = fcntl(connfd, F_GETFL, 0))) flags = 0;
			if(-1 == fcntl(connfd, F_SETFL, flags | O_NONBLOCK)){
				close(connfd);
				perror("Unable to set nonblocking");
			}
			// Add connection to a connection thread's queue
			struct epoll_event event;
			struct conn_state * state = malloc(sizeof(struct conn_state));
			if(state){
				state->fd = connfd;
				state->pos = 0;
				event.events = EPOLLIN|EPOLLRDHUP|EPOLLHUP|EPOLLERR;
				event.data.ptr = state;
				if(epoll_ctl(epollfd[j], EPOLL_CTL_ADD, connfd, &event)){
					close(connfd);
					perror("epoll_ctl");
				}
				j = (j + 1) % CONNECTION_THREADS;
			}
			else{
				perror("Malloc");
				close(connfd);
			}
		}else{
			perror("Accept");
		}
	}
}

int main(int argc, char **argv){
	int opt;
	while((opt = getopt(argc, argv, "p:R:s")) > 0){
		switch(opt){
			case 'p':
				port = optarg;
				break;
			case 'R':
				fpath = optarg;
				break;
			case 's':
				logging = false;
				break;
			default:
				fprintf(stderr, "Unknown option: %c\n", opt);
				print_usage();
				return 1;
		}
	}
	if(port == NULL){
		fprintf(stderr, "Port number not specified\n");
		print_usage();
		return 1;
	}
	if(logging) printf("Port: %s Directory: %s\n", port, fpath);
	signal(SIGPIPE, SIG_IGN);
	int listenfd = get_listen_fd(port);
	if(listenfd < 0){
		fprintf(stderr, "Unable to open socket\n");
		return 1;
	}
	for(int i = 0; i < ACCEPT_THREADS_MAX; ++i){
		pthread_t accept_thread;
		if(pthread_create(&accept_thread, NULL, &accept_routine,
				(void*)(intptr_t) listenfd)){
			fprintf(stderr, "Error creating accept threads.\n");
			break;
		}

	}
	accept_routine((void*)(intptr_t) listenfd);
	close(listenfd);
}

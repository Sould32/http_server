#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <netdb.h>
#include <signal.h>
#include "serve_static.h"
#include "sysstatd.h"
#include "system_info.h"
#include "artificial_loading.h"
#include "sockets.h"
#include "http_request.h"
#include "http_response.h"
#include <pthread.h>
static void print_usage(){
	fprintf(stderr, "Usage: sysstatd -p [PORT NUMBER] -R [STATIC PATH]\n");
	fprintf(stderr, "    -p    Specify the port to host the server on.\n");
	fprintf(stderr, 
			"    -R    Specifiy the directory to host static files from\n");
}

char * fpath = NULL;
char * port = NULL;
bool logging = true;

void * connection_routine(void * fd){
	int connfd = (intptr_t) fd;
	//Handle connection
	if(logging) printf("Accepted connection: %d\n", connfd);
	while(! read_request(connfd)){
		//Empty loop
	}	
	close(connfd);
	if(logging) printf("Connection closed: %d\n", connfd);
	return NULL;
}

void conncection_thread(int fd){
	pthread_t thread;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, &connection_routine, (void*)(intptr_t) fd)){
		fprintf(stderr, "Thread spawn error\n");
		response_head(fd, HTTP_SERVICE_UNAVAILABLE, "Unable to spawn thread"); 
	}
	pthread_attr_destroy(&thread_attr);
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
	while(1){
		int connfd;
		struct sockaddr_storage clientaddr;
		socklen_t clientaddr_len = sizeof(clientaddr);
		connfd = accept(listenfd, (struct sockaddr *) &clientaddr, 
				&clientaddr_len);
		if(connfd > 0){
			// spawn off a new thread to handle that connection
			conncection_thread(connfd);
		}else{
			perror("Accept");
		}
	}
	close(listenfd);
}

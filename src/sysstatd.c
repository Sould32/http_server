#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include "serve_static.h"
#include "sysstatd.h"
#include "system_info.h"
#include "artificial_loading.h"
#include "sockets.h"

static void print_usage(){
	fprintf(stderr, "Usage: sysstatd -p [PORT NUMBER] -R [STATIC PATH]\n");
	fprintf(stderr, "    -p    Specify the port to host the server on.\n");
	fprintf(stderr, 
			"    -R    Specifiy the directory to host static files from\n");
}

char * fpath = NULL;
char * port = NULL;
bool logging = true;

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
	if(fpath == NULL){
		fprintf(stderr, "Static directory not specified\n");
		print_usage();
		return 1;
	}
	if(logging) printf("Port: %s Directory: %s\n", port, fpath);
	int listenfd = get_listen_fd(port);
	if(listenfd < 0){
		fprintf(stderr, "Unable to open socket\n");
		return 1;
	}
	close(listenfd);
}

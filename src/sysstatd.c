#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "sysstatd.h"

static void print_usage(){
	fprintf(stderr, "Usage: sysstatd -p [PORT NUMBER] -R [STATIC PATH]\n");
	fprintf(stderr, "    -p    Specify the port to host the server on.\n");
	fprintf(stderr, 
			"    -R    Specifiy the directory to host static files from\n");
}

char * fpath = NULL;
int port = -1;

int main(int argc, char **argv){
	int opt;
	while((opt = getopt(argc, argv, "p:R:")) > 0){
		switch(opt){
			case 'p':
				if(sscanf(optarg, "%d", &port) != 1 || port < 0){
					fprintf(stderr, "Invalid port number specified\n");
				}
				break;
			case 'R':
				//Warning: Unbounded strcpy use! Safe due to dynamic allocation
				//         of sufficient memory.
				fpath = malloc(strlen(optarg) + 1);
				assert(fpath);
				strcpy(fpath, optarg);
				break;
			default:
				fprintf(stderr, "Unknown option: %c\n", opt);
				print_usage();
				return 2;
		}
	}
	if(port == -1){
		fprintf(stderr, "Port number not specified\n");
		print_usage();
		return 1;
	}
	if(fpath == NULL){
		fprintf(stderr, "Static directory not specified\n");
		print_usage();
		return 1;
	}
}
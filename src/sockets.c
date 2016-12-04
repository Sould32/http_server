#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


int get_listen_fd(char * port){
	struct addrinfo hints, *res;
	int on = 1, off = 0;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_ADDRCONFIG;
	if(getaddrinfo(NULL, port, &hints, &res)){
		fprintf(stderr, "Unable to look up address");
		return -1;
	}
	int sock = -1;
	for(struct addrinfo * p = res; p != NULL; p = p->ai_next){
		if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
			continue;
		}
		if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(int))){
			fprintf(stderr, "Unable to set socket option SO_REUSEADDR\n");
			close(sock);
			continue;
		}
		if(setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, 
				(void*)&off, sizeof(int))){
			fprintf(stderr, "Unable to unset socket option IPV6_V6ONLY\n");
			close(sock);
			continue;
		}
		if(! bind(sock, p->ai_addr, p->ai_addrlen)){
			break;
		}
		close(sock);
	}
	freeaddrinfo(res);
	return sock;
}

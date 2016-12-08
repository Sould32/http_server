#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "sockets.h"

extern bool logging;

void response(int fd, char *msg, char *content, size_t content_len){
	if(logging) printf("Sending response\n");
	write_to_socket(fd, "HTTP/1.1 ", strlen("HTTP/1.1 "));
	write_to_socket(fd, msg, strlen(msg));
	write_to_socket(fd, "\r\n", 2);
	//TODO handle header field
	char res[100];
	snprintf(res, 100, "Content-Length: %zu\r\n", content_len);
	write_to_socket(fd, res, strlen(res));
	write_to_socket(fd, "\r\n", 2);
	write_to_socket(fd, content, content_len);
}

void response_head(int fd, char* msg, char* content){
	 response(fd, msg, content, strlen(content));
}


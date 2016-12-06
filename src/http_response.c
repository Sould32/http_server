#include <string.h>
#include "sockets.h"
#include <stdio.h>

void response(int fd, char *msg, char *content, size_t content_len){
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

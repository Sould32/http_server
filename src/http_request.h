#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

enum http_request_method {
	OPTIONS,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	TRACE,
	CONNECT,
	UNKNOWN
};

#define REQUEST_BUFFER_SIZE 4096

struct conn_state {
	char buffer[REQUEST_BUFFER_SIZE];
	int pos;
	int fd;
};

int read_request(struct conn_state * state);

#endif

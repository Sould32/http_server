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

int read_request(int fd);

#endif

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

struct http_header_field {
	char * name;
	char * value;
	struct http_header_field * next;
};

struct http_request {
	enum http_request_method method;
	char * uri;
	size_t content_len;
	struct http_header_field * headers;
};

int read_request(int fd);

#endif

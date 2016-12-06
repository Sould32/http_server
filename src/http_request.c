#include <stdio.h>
#include <string.h>
#include "sockets.h"
#include "http_request.h"
#include "http_response.h"

#define MAX_LINE_LENGTH 2048

static enum http_request_method parse_method(char* line){
	if(strlen(line) < 3){
		return UNKNOWN;
	}
	switch(line[0]){
		case 'O':
			return OPTIONS;
		case 'H':
			return HEAD;
		case 'G':
			return GET;
		case 'P':
			if(line[1] == 'O')
				return POST;
			else if(line[1] == 'U')
				return PUT;
			else
				return UNKNOWN;
		case 'D':
			return DELETE;
		case 'T':
			return TRACE;
		case 'C':
			return CONNECT;
		default:
			return UNKNOWN;
	}
}

static void free_request(struct http_request * req){
	struct http_header_field * prev = NULL;
	for(struct http_header_field * head = req->headers; head != NULL;
			head = head->next){
		free(head->name);
		free(head->value);
		if(prev != NULL) free(prev);
		prev = head;
	}
	if(prev != NULL) free(prev);
}

static char * bad_request_msg = "Error 400: Bad request";
static char * bad_uri_msg = "Error 414: URI too long to read";
static char * http_version_msg = "Unknown HTTP version; only 1.1 supported";
static char * bad_request_type = "Request type not implemented";
static char * no_length_msg = "Length of request body required";
static char * not_found_msg = "Not found";

int read_request(int fd){
	struct http_request req;
	char line[MAX_LINE_LENGTH];
	int nread = 0;
	if((nread = socket_read_line(fd, line, MAX_LINE_LENGTH)) < 0){
		return 1;
	}
	if(nread == EOF) {printf("EOF encountered!"); return 1;}
	printf("Header line: %s\n", line);
	//Split at start of URI
	char * uri;
	if((uri = strchr(line, ' ')) == NULL){
		//Send back bad request
		response(fd, HTTP_BAD_REQUEST, bad_request_msg, strlen(bad_request_msg));
		return 1;
	}
	else{
		*uri = '\0';
		uri++;
	}
	//Split at start of http version
	char * http_version;
	if((http_version = strchr(uri, ' ')) == NULL){
		//Send back uri too large
		response(fd, HTTP_URI_TOO_LARGE, bad_uri_msg, strlen(bad_uri_msg));
		return 1;
	}
	else{
		*http_version = '\0';
		http_version++;
	}
	//Check http version
	if(strcmp(http_version, "HTTP/1.1")){// May need to accept 1.0 too
		//Send back http version not supported
		response(fd, HTTP_VERSION_NOT_SUPPORTED, http_version_msg, strlen(http_version_msg));
		return 1;
	}
	//Check request method
	req.method = parse_method(line);
	if(req.method == UNKNOWN){
		//Send back unimplemented
		response(fd, HTTP_NOT_IMPLEMENTED, bad_request_type, strlen(bad_request_type));
		return 1;
	}
	//Parse URI
	printf("%s\n", uri);
	if(uri[0] != '/'){
		//Hostname was included
		uri = strchr(uri, '/');
		if(uri == NULL){
			response(fd, HTTP_NOT_FOUND, not_found_msg, strlen(not_found_msg));
			return 1;
		}
	}
	//Read header fields
	char header_line[MAX_LINE_LENGTH];
	req.content_len = -1;
	do{
		if(socket_read_line(fd, header_line, MAX_LINE_LENGTH) < 0){
			return 1;
		}
		//Store content-length directly
		if(strstr(line, "Content-Length: ") == line){
			if(sscanf(line, "%*s %zd", & req.content_len) < 1){
				response(fd, HTTP_BAD_REQUEST, bad_request_msg, 
						strlen(bad_request_msg));
				free_request(& req);
				return 1;
			}
		}
		else{
			//Malloc request header
		}
	} while(strcmp(line, ""));
	if(req.method != HEAD && req.content_len == -1){
		response(fd, HTTP_LENGTH_REQUIRED, no_length_msg, strlen(no_length_msg));
	}
	//Pass request to appropriate handling function
	return 0;
}

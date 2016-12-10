#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "sockets.h"
#include "http_request.h"
#include "http_response.h"
#include "system_info.h"
#include "serve_static.h"
#include "artificial_loading.h"

#define MAX_LINE_LENGTH 2048

extern bool logging;

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

static char * bad_request_msg = "Error 400: Bad request";
static char * bad_uri_msg = "Error 414: URI too long to read";
static char * http_version_msg = "Unknown HTTP version; only 1.1 supported";
static char * bad_request_type = "Request type not implemented";
static char * no_length_msg = "Length of request body required";
static char * not_found_msg = "Not found";

int read_request(int fd){
	char line[MAX_LINE_LENGTH];
	int nread = 0;
	if((nread = socket_read_line(fd, line, MAX_LINE_LENGTH)) < 0){
		return 1;
	}
	if(nread == EOF) {if(logging) printf("EOF encountered!"); return 1;}
	if(logging) printf("Header line: %s\n", line);
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
	if(strcmp(http_version, "HTTP/1.1") && strcmp(http_version, "HTTP/1.0")){
		//Send back http version not supported
		response(fd, HTTP_VERSION_NOT_SUPPORTED, http_version_msg, strlen(http_version_msg));
		return 1;
	}
	bool close = false;
	if(!strcmp(http_version, "HTTP/1.0")){
		close = true;
	}
	//Check request method
	enum http_request_method method = parse_method(line);
	if(method == UNKNOWN){
		//Send back unimplemented
		response(fd, HTTP_NOT_IMPLEMENTED, bad_request_type, strlen(bad_request_type));
		return 1;
	}
	//Parse URI
	if(logging) printf("URI: %s\n", uri);
	char * callback_function = NULL;
	char * params = strchr(uri, '?');
	if(params){
		char * callback = strstr(uri, "?callback=");
		if(!callback) callback = strstr(uri, "&callback=");
		if(callback){
			callback += 10;
			char * end;
			if((end = strchr(callback, '&'))){
				*end = '\0';
			}
			bool isvalid = true;
			for(char * c = callback; *c != '\0'; ++c){
				isvalid &= isalnum(*c) || (*c == '.') || (*c == '_');
			}
			if(isvalid){
				callback_function = callback;
			}
		}
	}
	//Read header fields
	char header_line[MAX_LINE_LENGTH];
	int content_len = -1;
	do{
		if(socket_read_line(fd, header_line, MAX_LINE_LENGTH) < 0){
			return 1;
		}
		if(logging) printf("Header field: %s\n", header_line);
	} while(strlen(header_line) != 0);
	if(logging) printf("Header fields parsed\n");
	if(method != HEAD && method != GET && content_len == -1){
		response(fd, HTTP_LENGTH_REQUIRED, no_length_msg, strlen(no_length_msg));
	}
	//Pass request to appropriate handling function
	if(strstr(uri, "/loadavg") == uri && (uri[8] == '?' || uri[8] == '\0')){
		loadavg(fd, callback_function);
		return close;
	}
	else if(strstr(uri, "/meminfo") == uri && (uri[8] == '?' || uri[8] == '\0')){
		meminfo(fd, callback_function);
		return close;
	}
	else if(strstr(uri, "/files") == uri){
		uri = uri + 6; //Get rid of the files section
		serve_static(fd, uri);
		return close;
	}
	else if(strstr(uri, "/runloop") == uri && (uri[8] == '?' || uri[8] == '\0')){
		runloop(fd);
		return close;
	}
	else if(strstr(uri, "/allocanon") == uri && (uri[10] == '?' || uri[10] == '\0')){
		allocanon(fd);
		return close;
	}
	else if(strstr(uri, "/freeanon") == uri && (uri[9] == '?' || uri[9] == '\0')){
		freeanon(fd);
		return close;
	}
	else{
		response(fd, HTTP_NOT_FOUND, not_found_msg, strlen(not_found_msg));
		return 0;
	}
	//Should never hit this
	return 1;
}

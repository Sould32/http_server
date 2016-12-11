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

/*
 * TODO: Finish converting to epoll-based buffering
 * TODO: Remove http_request type; it's not useful
 * TODO: Break up read_request into more meaningful functions
 */

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

static int fill_buffer(struct conn_state * state){
	int n = read_from_socket(state->fd, state->buffer + state->pos,
			REQUEST_BUFFER_SIZE - state->pos - 1);
	if(n < 0){
		return 1;
	}
	state->pos += n;
	return 0;
}

static char * bad_request_msg = "Error 400: Bad request";
static char * bad_uri_msg = "Error 414: URI too long to read";
static char * http_version_msg = "Unknown HTTP version; only 1.1 supported";
static char * bad_request_type = "Request type not implemented";
//static char * no_length_msg = "Length of request body required";
static char * not_found_msg = "Not found";

int read_request(struct conn_state * state){
	if(logging) printf("Processing connection %d\n", state->fd);
	//Read data
	if(fill_buffer(state)) return 1;
	char * header_end = strstr(state->buffer, "\r\n\r\n");
	if(header_end == NULL){
		//Return if not ready or malformed
		if(state->pos == REQUEST_BUFFER_SIZE - 1) return 1; //Header too large
		else return 0;
	}
	//Process request
	header_end += 4;
	//Extract header line
	char * line = state->buffer;
	char * line_end = strstr(line, "\r\n");
	line_end[0] = '\0';
	if(logging) printf("Header line: %s\n", line);
	//Split at start of URI
	char * uri;
	if((uri = strchr(line, ' ')) == NULL){
		//Send back bad request
		response_head(state->fd, HTTP_BAD_REQUEST, bad_request_msg);
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
		response_head(state->fd, HTTP_URI_TOO_LARGE, bad_uri_msg);
		return 1;
	}
	else{
		*http_version = '\0';
		http_version++;
	}
	//Check http version
	if(strcmp(http_version, "HTTP/1.1") && strcmp(http_version, "HTTP/1.0")){
		//Send back http version not supported
		response_head(state->fd, HTTP_VERSION_NOT_SUPPORTED, http_version_msg);
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
		response_head(state->fd, HTTP_NOT_IMPLEMENTED, bad_request_type);
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
	#if 0
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
	#endif
	//Pass request to appropriate handling function
	if(strstr(uri, "/loadavg") == uri && (uri[8] == '?' || uri[8] == '\0')){
		loadavg(state->fd, callback_function);
	}
	else if(strstr(uri, "/meminfo") == uri && (uri[8] == '?' || uri[8] == '\0')){
		meminfo(state->fd, callback_function);
	}
	else if(strstr(uri, "/files") == uri){
		uri = uri + 6; //Get rid of the files section
		serve_static(state->fd, uri);
	}
	else if(strstr(uri, "/runloop") == uri && (uri[8] == '?' || uri[8] == '\0')){
		runloop(state->fd);
	}
	else if(strstr(uri, "/allocanon") == uri && (uri[10] == '?' || uri[10] == '\0')){
		allocanon(state->fd);
	}
	else if(strstr(uri, "/freeanon") == uri && (uri[9] == '?' || uri[9] == '\0')){
		freeanon(state->fd);
	}
	else{
		response_head(state->fd, HTTP_NOT_FOUND, not_found_msg);
	}
	//Clean up the buffer
	int offset = header_end - state->buffer;
	for(int i = 0; i < REQUEST_BUFFER_SIZE - offset; ++i){
		state->buffer[i] = state->buffer[i + offset];
	}
	state->pos -= offset;
	return close;
}

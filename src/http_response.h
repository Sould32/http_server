/*
 * Contains functions for writing an http response to a stream
 *
 * HTTP status codes and reason phrases from RFC 2616
 */

//Continuation codes
#define HTTP_CONTINUE "100 Continue"
#define HTTP_SWITCH_PROT "101 Switching Protocols"

//Success codes
#define HTTP_OK "200 OK"
#define HTTP_CREATED "201 Created"
#define HTTP_ACCEPTED "202 Accepted"
#define HTTP_NON_AUTHORITATIVE "203 Non-Authoritative Information"
#define HTTP_NO_CONTENT "204 No content"
#define HTTP_RESET_CONTENT "205 Reset Content"
#define HTTP_PARTIAL_CONTENT "206 Partial Content"

//Redirect codes
#define HTTP_MULT_CHOICES "300 Multiple Choices"
#define HTTP_MOVED_PERMANENTLY "301 Moved Permanently"
#define HTTP_FOUND "302 Found"
#define HTTP_SEE_OTHER "303 See Other"
#define HTTP_NOT_MODIFIED "304 Not Modified"
#define HTTP_USE_PROXY "305 Use Proxy"
//Code 306 not assigned
#define HTTP_TEMP_REDIRECT "307 Temporary Redirect"

//Client errors
#define HTTP_BAD_REQUEST "400 Bad Request"
#define HTTP_UNAUTHORIZED "401 Unauthorized"
#define HTTP_PAY_REQUIRED "402 Payment Required"
#define HTTP_FORBIDDEN "403 Forbidden"
#define HTTP_NOT_FOUND "404 Not Found"
#define HTTP_METHOD_NOT_ALLOWED "405 Method Not Allowed"
#define HTTP_NOT_ACCEPTABLE "406 Not Acceptable"
#define HTTP_PROXY_REQUIRED "407 Proxy Required"
#define HTTP_TIMED_OUT "408 Request Time-out"
#define HTTP_CONFLICT "409 Conflict"
#define HTTP_GONE "410 Gone"
#define HTTP_LENGTH_REQUIRED "411 Length Required"
#define HTTP_PRECON_FAILED "412 Precondition Failed"
#define HTTP_REQUEST_TOO_LARGE "413 Request Entity Too Large"
#define HTTP_URI_TOO_LARGE "414 Request-URI Too Large"
#define HTTP_UNSUPPORTED_TYPE "415 Unsupported Media Type"
#define HTTP_RANGE_NOT_SATISFIABLE "416 Requested range not satisfiable"
#define HTTP_EXPECTATION_FAILED "417 Expectation Failed"

//Server errors
#define HTTP_INTERNAL_ERROR "500 Internal Server Error"
#define HTTP_NOT_IMPLEMENTED "501 Not Implemented"
#define HTTP_BAD_GATEWAY "502 Bad Gateway"
#define HTTP_SERVICE_UNAVAILABLE "503 Service Unavailable"
#define HTTP_GATEWAY_TIMED_OUT "504 Gateway Time-out"
#define HTTP_VERSION_NOT_SUPPORTED "505 HTTP Version not supported"

void response(int fd, char *msg, char *content, size_t content_len);
void response_head(int fd, char* msg, char* content);
void send_response(int fd, char *msg, size_t content_len);

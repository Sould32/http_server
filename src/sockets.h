#ifndef SOCKETS_H
#define SOCKETS_H
#include <stdlib.h>
int get_listen_fd(char * port);
int read_from_socket(int socketfd, char* buff,  size_t num_byte);
int write_to_socket(int socketfd, char* buff, size_t num_byte);
int socket_read_line(int socketfd, char* buff, size_t max_length);
#endif

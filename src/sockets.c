#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int get_listen_fd(char * port){
	struct addrinfo hints, *res;
	int on = 1, off = 0;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_ADDRCONFIG;
	if(getaddrinfo(NULL, port, &hints, &res)){
		fprintf(stderr, "Unable to look up address");
		return -1;
	}
	int sock = -1;
	for(struct addrinfo * p = res; p != NULL; p = p->ai_next){
		if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
			continue;
		}
		if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(int))){
			fprintf(stderr, "Unable to set socket option SO_REUSEADDR\n");
			close(sock);
			continue;
		}
		if(setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, 
				(void*)&off, sizeof(int))){
			fprintf(stderr, "Unable to unset socket option IPV6_V6ONLY\n");
			close(sock);
			continue;
		}
		if(! bind(sock, p->ai_addr, p->ai_addrlen)){
			break;
		}
		close(sock);
	}
	freeaddrinfo(res);
	return sock;
}
int read_from_socket(int socketfd, char* buff,  size_t num_byte){
	bzero(buff, num_byte);
	size_t n_left = num_byte;
	ssize_t num_read;
	char* buff_pos = buff;
	while(n_left > 0){
		num_read = read(socketfd, buff_pos, n_left);
		if (num_read < 0){
			switch(errno){
				case EINTR:
					num_read = 0;
					break;
				case EAGAIN:
					perror("The O_NONBLOCKFLAG is set for this fd and the process will be delayed.\n");
					return -1;
				case EINVAL:
					perror("The fildes reference stream is linked downstream from a multiplexer.\n");
					return -1;
				case ECONNRESET:
					perror("A read was attempted on a socket and the connection was forcibly closed by its peer.\n");
					return -1;
				case ENOTCONN:
					perror("A read was attempted on a socket that is not connected.\n");
					return -1;
				case ETIMEDOUT:
					perror("A read was attempted on a socket and a tramission timeout occurred.\n");
					return -1;
			}
		}else if (num_read == 0){
			num_read = 0; // EOF
			break;
		}
		n_left -= num_read;
		buff_pos += num_read;
	}
	printf("%zu bytes successfully read from the socket\n", num_byte);
	return 0;
}
int write_to_socket(int socketfd, char* buff, size_t num_byte){
	bzero(buff, num_byte);
	size_t n_left = num_byte;
	char * buff_pos = buff;
	ssize_t num_written;

	while(n_left > 0){
	num_written = write(socketfd, buff_pos, n_left);
		if(num_written < 0){
			switch(errno){
				case EAGAIN:
					perror("The O_NONBLOCKFLAG is set for this fd and the process will be delayed.\n");
					return -1;
				case EDESTADDRREQ:
					perror("fd refers to a datagram socket for which a peer address has not been set using connect.\n");
					return -1;
				case EFAULT:
					perror("buffer is outside your accessible address space.\n");
					return -1;
				case EINTR:
					num_written = 0;
					break;
				case EPIPE:
					perror("The fd is connected to a socket whose reading end is closed.\n");
					return -1;
			}
		}
		n_left -= num_written;
		buff_pos += num_written;
	}
	printf("%zu bytes successffuly read.\n", num_byte);
	return 0 ;
}
int socket_read_line(int fd, char* buff, size_t max_length){
	int num_read;
	char c, *buff_pos = buff;
	int i;
	for (i = 0; (i < max_length); i++){
		if ((num_read = read(fd, &c, 1)) == 1){
			if(c == '\n' || c == '\r'){
				i++;
				break;
			}
			*buff_pos++ = c;
		}else if (num_read == 0){
			if (i == 1){
				return EOF; //EOF no data
			}else{
				break;
			}
		}else{
			return -1; //error
		}	
	}
	*buff_pos = 0;
	return 0;
}

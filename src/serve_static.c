#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include "http_response.h"

extern char * fpath;
extern bool logging;

const char * find_file(char* path, char *arg){
	DIR *dir;
	struct dirent *res;
	if ((dir = opendir(path)) != NULL){
		while ((res = readdir(dir)) != NULL){
			if (strstr(res->d_name, arg) != NULL){
				printf("%s\n", res->d_name);
				closedir(dir);
				return res->d_name;
			}
		}
		closedir(dir);
		return NULL;
	}else{
		perror("Error looking for the file");
	}
	return NULL;
}

void serve_static(int fd, char* path){
	if(strstr(path, "..")){
		response_head(fd, HTTP_FORBIDDEN, "Path may not contain ..");
		return;
	}
	if(fpath == NULL){
		response_head(fd, HTTP_NOT_FOUND, "Not currently serving files");
		return;
	}
	//Concatenate path. Duplicate /'s are not a problem
	char * fullpath = malloc(strlen(path) + strlen(fpath) + 1);
	strcpy(fullpath, fpath);
	strcpy(strchr(fullpath, '\0'), path);
	if(logging) printf("Serving file: %s\n", fullpath);
	FILE * file = fopen(fullpath, "r");
	if(file == NULL){
		
		perror("Unable to open file");
		response_head(fd, HTTP_NOT_FOUND, "File not found or inaccessible");
	}
	else{
		fseek(file, 0, SEEK_END);
		long int size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char * buf = malloc(size);
		bool err = false;
		if(fread(buf, sizeof(char), size, file) != size){
			err = true;
		}
		fclose(file);
		if(err){
			response_head(fd, HTTP_INTERNAL_ERROR, "Error reading file");
		}
		else{
			response(fd, HTTP_OK, buf, size);
		}
		free(buf);
	}
}

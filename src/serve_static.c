#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

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

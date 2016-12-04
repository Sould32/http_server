#ifndef SERVE_STATIC_H
#define SERVE_STATIC_H
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
// serve static h file
// exaple string.htm
// append it to R and and then search the entire directory
const char * find_file(char* path, char *arg);
#endif

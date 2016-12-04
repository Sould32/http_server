/*
 * Contains functions to implement a system info web service.
 */
#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <stdio.h>

void loadavg(FILE* fd);

void meminfo(FILE* fd);

#endif

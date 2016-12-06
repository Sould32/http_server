/*
 * Contains functions to implmement artifical loading and memory use
 */

#ifndef ARTIFICIAL_LOADING_H
#define ARTIFICIAL_LOADING_H

#include <stdio.h>

void runloop(int fd);

void allocanon(int fd);

void freeanon(int fd);

#endif

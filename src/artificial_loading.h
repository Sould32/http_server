/*
 * Contains functions to implmement artifical loading and memory use
 */

#ifndef ARTIFICIAL_LOADING_H
#define ARTIFICIAL_LOADING_H

#include <stdio.h>

void runloop(FILE * fd);

void allocanon(FILE * fd);

void freeanon(FILE * fd);

#endif

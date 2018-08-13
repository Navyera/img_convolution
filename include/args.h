#ifndef ARGS_H
#define ARGS_H

#include "pixel.h"

typedef struct {
    char *filename;
    int width;
    int height;
    color_t color;
    int loops;
} args_t;

void print_usage();
int parse_args(int argc, char *argv[], args_t *ret, int myrank);

#endif

#ifndef WORK_H
#define WORK_H

#include "args.h"
#include "neighbours.h"
#include "utils.h"

double work(MPI_Comm cart_comm, char *old_image, char *new_image, char **result_buf, args_t *args,
           NeighbourSet *neighbour, Block *myblock, unsigned int color_bytes);

#endif

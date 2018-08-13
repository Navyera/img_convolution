//
// Created by iliastsa on 8/6/18.
//

#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

typedef struct {
    int up;
    int down;
    int left;
    int right;
    int up_left;
    int up_right;
    int down_left;
    int down_right;
} NeighbourSet;

void calculate_neighbours(MPI_Comm *comm, NeighbourSet *neighbours, int coords[2], int procs_dim);

#endif

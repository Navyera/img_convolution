#include <mpi.h>
#include "neighbours.h"

#define CHECK_ASSIGN(N) if (temp[0] < 0 || temp[0] >= dims[0] || temp[1] < 0 || temp[1] >= dims[1]) {N = MPI_PROC_NULL;} else {MPI_Cart_rank(*comm, temp, &N);}

void calculate_neighbours(MPI_Comm *comm, NeighbourSet *neighbours, int coords[2], int procs_dim) {
    int dims[2] = {procs_dim, procs_dim};
    int temp[2];

    /*
     * Initialize temp to the left-neighbour coordinates, and start
     * moving clockwise.
     */

    // Left
    temp[0] = coords[0] - 1; temp[1] = coords[1];
    CHECK_ASSIGN(neighbours->left);

    // Up Left
    temp[1] -= 1;
    CHECK_ASSIGN(neighbours->up_left);

    //Up
    temp[0] += 1;
    CHECK_ASSIGN(neighbours->up);

    //Up Right
    temp[0] += 1;
    CHECK_ASSIGN(neighbours->up_right);

    //Right
    temp[1] += 1;
    CHECK_ASSIGN(neighbours->right);

    //Down Right
    temp[1] += 1;
    CHECK_ASSIGN(neighbours->down_right);

    //Down
    temp[0] -= 1;
    CHECK_ASSIGN(neighbours->down);

    //Down Left
    temp[0] -= 1;
    CHECK_ASSIGN(neighbours->down_left);
}


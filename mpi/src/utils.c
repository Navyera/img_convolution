#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "utils.h"
#include "pixel.h"
#include "args.h"
#include "directions.h"

// Helper function, taken from https://stackoverflow.com/questions/22859269/how-to-use-and-interpret-mpi-io-error-codes
void handle_error(int errcode, char *str) {
    char msg[MPI_MAX_ERROR_STRING];
    int resultlen;

    MPI_Error_string(errcode, msg, &resultlen);

    fprintf(stderr, "%s: %s\n", str, msg);

    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
}

int create_cartesian_comm(MPI_Comm *comm, int coord_2d[2], int myrank, int nprocs) {
    int procs_dim = (int) sqrt(nprocs);

    int periodic[2] = {0, 0};
    int dims[2] = {procs_dim, procs_dim};
    int reorder = 1;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, reorder, comm);
    MPI_Cart_coords(*comm, myrank, 2, coord_2d);

    return procs_dim;
}

int allocate_memory(char **old_image, char **new_image, int local_rows, int local_cols, unsigned int color_bytes) {
    *new_image  = calloc((local_cols + 2) * (local_rows + 2) * color_bytes, sizeof(char));
    *old_image  = calloc((local_cols + 2) * (local_rows + 2) * color_bytes, sizeof(char));

    if (*new_image == NULL || *old_image == NULL) {
        free(*new_image);
        free(*old_image);
        return -1;
    }

    return 0;
}

void calculate_offsets(int send_offsets[4], int recv_offsets[8], Block *myblock, unsigned int color_bytes) {
    const int local_cols = myblock->col_end - myblock->col_start;
    const int local_rows = myblock->row_end - myblock->row_start;

    send_offsets[UP_LEFT]    = LOCAL_OFFSET(0, 0, local_cols) * color_bytes;
    send_offsets[UP_RIGHT]   = LOCAL_OFFSET(0, local_cols-1, local_cols) * color_bytes;
    send_offsets[DOWN_RIGHT] = LOCAL_OFFSET(local_rows-1, local_cols-1, local_cols) * color_bytes;
    send_offsets[DOWN_LEFT]  = LOCAL_OFFSET(local_rows-1, 0, local_cols) * color_bytes;

    recv_offsets[UP_LEFT]    = LOCAL_OFFSET(-1, -1, local_cols) * color_bytes;
    recv_offsets[UP_RIGHT]   = LOCAL_OFFSET(-1, local_cols, local_cols) * color_bytes;
    recv_offsets[DOWN_RIGHT] = LOCAL_OFFSET(local_rows, local_cols, local_cols) * color_bytes;
    recv_offsets[DOWN_LEFT]  = LOCAL_OFFSET(local_rows, -1, local_cols) * color_bytes;

    recv_offsets[LEFT]       = LOCAL_OFFSET(0, -1, local_cols) * color_bytes;
    recv_offsets[RIGHT]      = LOCAL_OFFSET(0, local_cols, local_cols) * color_bytes;
    recv_offsets[UP]         = LOCAL_OFFSET(-1, 0, local_cols) * color_bytes;
    recv_offsets[DOWN]       = LOCAL_OFFSET(local_rows, 0, local_cols) * color_bytes;
}

void get_block(Block *block, args_t *args, int *coord_2d, int procs_dim) {
    // Conditions for enabling ceiling matrix division
    const int ceil_cond_width = ((procs_dim-1) * CEIL_DIV(args->width, procs_dim) < args->width);
    const int ceil_cond_height = ((procs_dim-1) * CEIL_DIV(args->height, procs_dim) < args->height);

    int blk_width  = ceil_cond_width  ? CEIL_DIV(args->width, procs_dim)  : (args->width/procs_dim);
    int blk_height = ceil_cond_height ? CEIL_DIV(args->height, procs_dim) : (args->height/procs_dim);

    block->col_start = coord_2d[0] * blk_width;
    block->col_end   = (coord_2d[0] + 1) * blk_width;

    block->row_start = coord_2d[1] * blk_height;
    block->row_end   = (coord_2d[1] + 1) * blk_height;

    if (coord_2d[0] == procs_dim - 1)
        block->col_end = args->width;

    if (coord_2d[1] == procs_dim - 1)
        block->row_end = args->height;
}

char is_perfect_square(int n) {
    int sqrt_n = (int) sqrt(n);

    return (sqrt_n * sqrt_n) == n;
}
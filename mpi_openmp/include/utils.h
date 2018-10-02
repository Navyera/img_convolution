#ifndef UTILS_H
#define UTILS_H

#define CEIL_DIV(X,Y) (((X) + (Y) - 1)/(Y))

// Macros used for offset calculation in different contexts
#define GLOBAL_OFFSET(ROW,COL,W) (((ROW) * (W)) + (COL))
#define LOCAL_OFFSET(ROW,COL,W) ((((ROW) +  1 ) * ((W) + 2)) + ((COL) + 1))

// Useful macros for master distinction and error printing
#define IF_MASTER_ERR(RANK,DO) {if ((RANK) == 0) DO return -1;}
#define DUPLICATE_ERR(ARG) { printf("Error : Duplicate argument {"ARG"}.\n");}

// Macro used for error checking in MPI methods. Code taken from:
// https://stackoverflow.com/questions/22859269/how-to-use-and-interpret-mpi-io-error-codes
#define MPI_CHECK(fn) { int errcode; errcode = (fn);\
     if (errcode != MPI_SUCCESS) handle_error  (errcode, #fn ); }

#include <mpi.h>
#include "args.h"

typedef struct {
    int col_start;
    int col_end;
    int row_start;
    int row_end;
} Block;

void handle_error(int errcode, char *str);
int create_cartesian_comm(MPI_Comm *comm, int coord_2d[2], int myrank, int nprocs);
void get_block(Block *block, args_t *args, int *coord_2d, int procs_dim);
int allocate_memory(char **old_image, char **new_image, int local_rows, int local_cols, unsigned int color_bytes);
void calculate_offsets(int send_offsets[4], int recv_offsets[8], Block *myblock, unsigned int color_bytes);
char is_perfect_square(int n);

#endif

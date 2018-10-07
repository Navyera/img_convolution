#include <math.h>
#include <time.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mpi.h>

#include "args.h"
#include "utils.h"
#include "io.h"
#include "work.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc,&argv);

    int myrank;
    int nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (!is_perfect_square(nprocs)) {
        if (myrank == 0)
            printf("Number of processes must be a perfect square.\n");

        MPI_Finalize();

        return EXIT_FAILURE;
    }

    args_t args;

    if (parse_args(argc, argv, &args, myrank) < 0) {
        if (myrank == 0)
            print_usage();

        MPI_Finalize();

        return EXIT_FAILURE;
    }

    MPI_Comm cart_comm;

    int coord_2d[2];
    int procs_dim = create_cartesian_comm(&cart_comm, coord_2d, myrank, nprocs);

    NeighbourSet neighbour;
    calculate_neighbours(&cart_comm, &neighbour, coord_2d, procs_dim);

    Block myblock;
    get_block(&myblock, &args, coord_2d, procs_dim);

    // Local dimension sizes
    const int local_cols = myblock.col_end - myblock.col_start;
    const int local_rows = myblock.row_end - myblock.row_start;

    // Global dimension sizes
    const int global_cols = args.width;

    // Color scale factor
    const unsigned int color_bytes = args.color;

    char *old_image, *new_image;
    char *old_ptr, *new_ptr;
    if (allocate_memory(&old_image, &new_image, local_rows, local_cols, color_bytes) < 0) {
        MPI_Abort(cart_comm, EXIT_FAILURE);

        return EXIT_FAILURE;
    }
    old_ptr = old_image;
    new_ptr = new_image;

    read_block(cart_comm, args.filename, old_image, &myblock, global_cols, color_bytes);

    double duration =  work(cart_comm, old_image, new_image, &new_image, &args, &neighbour, &myblock, color_bytes);

    if (myrank == 0)
        printf("%f\n", duration);

    //write_block(cart_comm, "result.raw", new_image, &myblock, global_cols, color_bytes);

    free(old_ptr);
    free(new_ptr);
    MPI_Finalize();

    return EXIT_SUCCESS;
}

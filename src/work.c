#include <time.h>
#include <mpi.h>

#include "work.h"
#include "directions.h"
#include "args.h"
#include "utils.h"
#include "neighbours.h"
#include "filters.h"
#include "convolute.h"

float work(MPI_Comm cart_comm, char *old_image, char *new_image, char **result_buf, args_t *args,
           NeighbourSet *neighbour, Block *myblock, unsigned int color_bytes) {
    const int local_cols = myblock->col_end - myblock->col_start;
    const int local_rows = myblock->row_end - myblock->row_start;
    
    // Vector datatype for columns
    MPI_Datatype cols_datatype;
    MPI_Type_vector(local_rows, color_bytes, (local_cols + 2) * color_bytes, MPI_UNSIGNED_CHAR, &cols_datatype);
    MPI_Type_commit(&cols_datatype);

    // Contiguous datatype for rows
    MPI_Datatype rows_datatype;
    MPI_Type_contiguous(color_bytes * local_cols, MPI_UNSIGNED_CHAR, &rows_datatype);
    MPI_Type_commit(&rows_datatype);

    int send_offsets[4], recv_offsets[8];
    calculate_offsets(send_offsets, recv_offsets, myblock, color_bytes);

    // Arrays to store communication handlers
    MPI_Request send_requests[8];
    MPI_Request recv_requests[8];
    
    time_t t0 = clock();

    for (int i = 0; i < args->loops; ++i) {
        // Send column vectors to left and right neighbours
        MPI_Isend(&old_image[send_offsets[UP_LEFT]], 1, cols_datatype, neighbour->left, 0, cart_comm, &send_requests[LEFT]);
        MPI_Isend(&old_image[send_offsets[UP_RIGHT]], 1, cols_datatype, neighbour->right, 0, cart_comm, &send_requests[RIGHT]);

        // Send row data to up and down neighbours
        MPI_Isend(&old_image[send_offsets[UP_LEFT]], 1, rows_datatype, neighbour->up, 0, cart_comm, &send_requests[UP]);
        MPI_Isend(&old_image[send_offsets[DOWN_LEFT]], 1, rows_datatype, neighbour->down, 0, cart_comm, &send_requests[DOWN]);


        // Send corner data to corner neighbours
        MPI_Isend(&old_image[send_offsets[UP_LEFT]],    color_bytes, MPI_UNSIGNED_CHAR, neighbour->up_left,    0, cart_comm, &send_requests[UP_LEFT]);
        MPI_Isend(&old_image[send_offsets[UP_RIGHT]],   color_bytes, MPI_UNSIGNED_CHAR, neighbour->up_right,   0, cart_comm, &send_requests[UP_RIGHT]);
        MPI_Isend(&old_image[send_offsets[DOWN_RIGHT]], color_bytes, MPI_UNSIGNED_CHAR, neighbour->down_right, 0, cart_comm, &send_requests[DOWN_RIGHT]);
        MPI_Isend(&old_image[send_offsets[DOWN_LEFT]],  color_bytes, MPI_UNSIGNED_CHAR, neighbour->down_left,  0, cart_comm, &send_requests[DOWN_LEFT]);


        // Receive column vectors from left and right neighbours
        MPI_Irecv(&old_image[recv_offsets[LEFT]], 1, cols_datatype, neighbour->left, 0, cart_comm, &recv_requests[LEFT]);
        MPI_Irecv(&old_image[recv_offsets[RIGHT]], 1, cols_datatype, neighbour->right, 0, cart_comm, &recv_requests[RIGHT]);

        // Receive row data from up and down neighbours
        MPI_Irecv(&old_image[recv_offsets[UP]], 1, rows_datatype, neighbour->up, 0, cart_comm, &recv_requests[UP]);
        MPI_Irecv(&old_image[recv_offsets[DOWN]], 1, rows_datatype, neighbour->down, 0, cart_comm, &recv_requests[DOWN]);

        // Receive corner data from corner neighbours
        MPI_Irecv(&old_image[recv_offsets[UP_LEFT]],    color_bytes, MPI_UNSIGNED_CHAR, neighbour->up_left,    0, cart_comm, &recv_requests[UP_LEFT]);
        MPI_Irecv(&old_image[recv_offsets[UP_RIGHT]],   color_bytes, MPI_UNSIGNED_CHAR, neighbour->up_right,   0, cart_comm, &recv_requests[UP_RIGHT]);
        MPI_Irecv(&old_image[recv_offsets[DOWN_RIGHT]], color_bytes, MPI_UNSIGNED_CHAR, neighbour->down_right, 0, cart_comm, &recv_requests[DOWN_RIGHT]);
        MPI_Irecv(&old_image[recv_offsets[DOWN_LEFT]],  color_bytes, MPI_UNSIGNED_CHAR, neighbour->down_left,  0, cart_comm, &recv_requests[DOWN_LEFT]);

        // Convolve inner block
        convolve(old_image, new_image, args->color, 1, local_rows - 1, 1, local_cols - 1, local_cols, FILTER);

        // Finalize recv(s) from neighbours and convolve the outer pixels, that require halo points

        // Receive left and right columns and convolve
        MPI_Wait(&recv_requests[LEFT], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, 1, local_rows - 1, 0, 1, local_cols, FILTER);

        MPI_Wait(&recv_requests[RIGHT], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, 1, local_rows - 1, local_cols - 1, local_cols, local_cols, FILTER);

        // Receive up and down rows and convolve
        MPI_Wait(&recv_requests[UP], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, 0, 1, 1, local_cols - 1, local_cols, FILTER);

        MPI_Wait(&recv_requests[DOWN], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, local_rows - 1, local_rows, 1, local_cols - 1, local_cols, FILTER);

        // Receive corner data and convolve
        MPI_Wait(&recv_requests[UP_LEFT], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, 0, 1, 0, 1, local_cols, FILTER);

        MPI_Wait(&recv_requests[UP_RIGHT], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, 0, 1, local_cols - 1, local_cols, local_cols, FILTER);

        MPI_Wait(&recv_requests[DOWN_RIGHT], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, local_rows - 1, local_rows, local_cols - 1, local_cols, local_cols, FILTER);

        MPI_Wait(&recv_requests[DOWN_LEFT], MPI_STATUS_IGNORE);
        convolve(old_image, new_image, args->color, local_rows - 1, local_rows, 0, 1, local_cols, FILTER);

        // Finalize send
        for (int j = 0; j < 8; ++j)
            MPI_Wait(&send_requests[j], MPI_STATUS_IGNORE);

        if (i == args->loops - 1)
            break;

        char *tmp = old_image;
        old_image = new_image;
        new_image = tmp;
    }
    
    time_t t1 = clock();

    *result_buf = new_image;
    
    return ((float)(t1-t0))/CLOCKS_PER_SEC;
}
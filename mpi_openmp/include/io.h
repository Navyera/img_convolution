#ifndef IO_H
#define IO_H

void read_block(MPI_Comm cart_comm, char *filename, char *old_image, Block *myblock, int global_cols, unsigned int color_bytes);
void write_block(MPI_Comm cart_comm, char *filename, char *new_image, Block *myblock, int global_cols, unsigned int color_bytes);
#endif

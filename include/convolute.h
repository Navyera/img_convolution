#ifndef CONVOLUTE_H
#define CONVOLUTE_H

void convolve(char *old_image, char *new_image, color_t color, int row_from, int row_to, int col_from, int col_to,
              int width, float filter[3][3]);
#endif

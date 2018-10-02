#include "pixel.h"
#include "utils.h"
#include "args.h"

//

void apply_filter_rgb_fast(char *pxl, char *dest, int i, int j, int width, float filter[3][3]) {
    // Cast parameters, so the address is indexed properly
    RGB_Pixel *old = (RGB_Pixel*) pxl;
    RGB_Pixel *new = (RGB_Pixel*) dest;

    int r, g, b;
    r = (int) (old[LOCAL_OFFSET(i-1, j-1, width)].r * filter[2][2] + old[LOCAL_OFFSET(i-1, j, width)].r * filter[2][1] + old[LOCAL_OFFSET(i-1, j+1, width)].r * filter[2][0]
             + old[LOCAL_OFFSET( i , j-1, width)].r * filter[1][2] + old[LOCAL_OFFSET( i , j, width)].r * filter[1][1] + old[LOCAL_OFFSET( i , j+1, width)].r * filter[1][0]
             + old[LOCAL_OFFSET(i+1, j-1, width)].r * filter[0][2] + old[LOCAL_OFFSET(i+1, j, width)].r * filter[0][1] + old[LOCAL_OFFSET(i+1, j+1, width)].r * filter[0][0]);

    g = (int) (old[LOCAL_OFFSET(i-1, j-1, width)].g * filter[2][2] + old[LOCAL_OFFSET(i-1, j, width)].g * filter[2][1] + old[LOCAL_OFFSET(i-1, j+1, width)].g * filter[2][0]
             + old[LOCAL_OFFSET( i , j-1, width)].g * filter[1][2] + old[LOCAL_OFFSET( i , j, width)].g * filter[1][1] + old[LOCAL_OFFSET( i , j+1, width)].g * filter[1][0]
             + old[LOCAL_OFFSET(i+1, j-1, width)].g * filter[0][2] + old[LOCAL_OFFSET(i+1, j, width)].g * filter[0][1] + old[LOCAL_OFFSET(i+1, j+1, width)].g * filter[0][0]);

    b = (int) (old[LOCAL_OFFSET(i-1, j-1, width)].b * filter[2][2] + old[LOCAL_OFFSET(i-1, j, width)].b * filter[2][1] + old[LOCAL_OFFSET(i-1, j+1, width)].b * filter[2][0]
             + old[LOCAL_OFFSET( i , j-1, width)].b * filter[1][2] + old[LOCAL_OFFSET( i , j, width)].b * filter[1][1] + old[LOCAL_OFFSET( i , j+1, width)].b * filter[1][0]
             + old[LOCAL_OFFSET(i+1, j-1, width)].b * filter[0][2] + old[LOCAL_OFFSET(i+1, j, width)].b * filter[0][1] + old[LOCAL_OFFSET(i+1, j+1, width)].b * filter[0][0]);

    new[LOCAL_OFFSET(i,j,width)].r = (unsigned char)(r < 0 ? 0 : r > 255 ? 255 : r);
    new[LOCAL_OFFSET(i,j,width)].g = (unsigned char)(g < 0 ? 0 : g > 255 ? 255 : g);
    new[LOCAL_OFFSET(i,j,width)].b = (unsigned char)(b < 0 ? 0 : b > 255 ? 255 : b);
}

void apply_filter_gray_fast(char *pxl, char *dest, int i, int j, int width, float filter[3][3]) {
    // Cast parameters, so the address is indexed properly
    Gray_Pixel *old = (Gray_Pixel*) pxl;
    Gray_Pixel *new = (Gray_Pixel*) dest;

    int c;
    c = (int) (old[LOCAL_OFFSET(i-1, j-1, width)].c * filter[2][2] + old[LOCAL_OFFSET(i-1, j, width)].c * filter[2][1] + old[LOCAL_OFFSET(i-1, j+1, width)].c * filter[2][0]
               + old[LOCAL_OFFSET( i , j-1, width)].c * filter[1][2] + old[LOCAL_OFFSET( i , j, width)].c * filter[1][1] + old[LOCAL_OFFSET( i , j+1, width)].c * filter[1][0]
               + old[LOCAL_OFFSET(i+1, j-1, width)].c * filter[0][2] + old[LOCAL_OFFSET(i+1, j, width)].c * filter[0][1] + old[LOCAL_OFFSET(i+1, j+1, width)].c * filter[0][0]);
    
    new[LOCAL_OFFSET(i,j,width)].c = (unsigned char)(c < 0 ? 0 : c > 255 ? 255 : c);
}

void apply_filter_rgb_loop(char *pxl, char *dest, int i, int j, int width, float filter[3][3]) {
    // Cast parameters, so the address is indexed properly
    RGB_Pixel *old = (RGB_Pixel*) pxl;
    RGB_Pixel *new = (RGB_Pixel*) dest;

    float r = 0, g = 0, b = 0;

    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            int offset = LOCAL_OFFSET(i-y,j-x,width);

            r += old[offset].r * filter[1-y][1-x];
            g += old[offset].g * filter[1-y][1-x];
            b += old[offset].b * filter[1-y][1-x];
        }
    }

    new[LOCAL_OFFSET(i,j,width)].r = (unsigned char)(r < 0 ? 0 : r > 255 ? 255 : r);
    new[LOCAL_OFFSET(i,j,width)].g = (unsigned char)(g < 0 ? 0 : g > 255 ? 255 : g);
    new[LOCAL_OFFSET(i,j,width)].b = (unsigned char)(b < 0 ? 0 : b > 255 ? 255 : b);
}

void apply_filter_gray_loop(char *pxl, char *dest, int i, int j, int width, float filter[3][3]){
    // Cast parameters, so the pixel arrays are indexed properly
    Gray_Pixel *old = (Gray_Pixel*) pxl;
    Gray_Pixel *new = (Gray_Pixel*) dest;

    float c = 0;

    for (int y = -1; y <= 1; ++y)
        for (int x = -1; x <= 1; ++x)
            c += old[LOCAL_OFFSET(i-y,j-x,width)].c * filter[1-y][1-x];

    new[LOCAL_OFFSET(i,j,width)].c = (unsigned char)(c < 0 ? 0 : c > 255 ? 255 : c);
}

void convolve(char *old_image, char *new_image, color_t color, int row_from, int row_to, int col_from, int col_to,
              int width, float filter[3][3]) {
    void (*apply_fun)(char*, char *, int, int, int, float[3][3]);

    if (color == RGB)
        apply_fun = apply_filter_rgb_fast;
    else
        apply_fun = apply_filter_gray_fast;

    for (int i = row_from; i < row_to; ++i)
        for (int j = col_from; j < col_to; ++j)
            apply_fun(old_image, new_image, i, j, width, filter);
}


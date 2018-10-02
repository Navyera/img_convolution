#ifndef PIXEL_H
#define PIXEL_H

typedef enum {
    GRAYSCALE = 1,
    RGB       = 3
} color_t;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB_Pixel;

typedef struct {
    unsigned char c;
}Gray_Pixel;

#endif

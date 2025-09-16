#pragma once

#include <stdint.h>

// Pixel struc
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;


// Image struct use **of Pixels to store data
typedef struct
{
    int width, height;
    Pixel **pixels;
} Image;

// func
Image *createImage(int width, int height);
Pixel *getPixel(Image *img, int x, int y);
void setPixel(Image *img, int x, int y, Pixel *p);
Image *loadImage(const char *file);
void exportImage(Image *img, const char *file);

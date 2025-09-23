#pragma once

#include <SDL2/SDL_image.h>
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
Pixel *getPixel(const Image *img, int x, int y);
void setPixel(const Image *img, int x, int y, const Pixel *p);
Image *loadImage(const char *file);
void exportImage(SDL_Surface *surf, const char *file);
SDL_Surface *getSDL_Surface(const Image *img);
Image *copyImage(const Image *img);
Image *rotateImage(const Image *src, double angle);

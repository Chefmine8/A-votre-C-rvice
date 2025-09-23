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
Image *create_image(int width, int height);
Pixel *get_pixel(const Image *img, int x, int y);
void set_pixel(const Image *img, int x, int y, const Pixel *p);
Image *load_image(const char *file);
void export_image(SDL_Surface *surf, const char *file);
SDL_Surface *get_sdl_surface(const Image *img);
Image *copy_image(const Image *img);
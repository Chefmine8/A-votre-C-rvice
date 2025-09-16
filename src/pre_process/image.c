#include "image.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

// use to alloc memory for a new image
Image *createImage(int width, int height)
{
    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;

    img->pixels = malloc(height * sizeof(Pixel *));
    for (int y = 0; y < height; y++)
    {
        img->pixels[y] = malloc(width * sizeof(Pixel));
    }

    return img;
}

// get pixel at (x, y) return struct Pixel
Pixel *getPixel(Image *img, int x, int y) 
{
    if (!(x >= 0 && x < img->width && y >= 0 && y < img->height))
    {
        printf("Cannot read pixel (x:%d, y:%d)\n", x, y);
        exit(EXIT_FAILURE);
    }
    return &img->pixels[y][x];
}

// change tu pixel at (x, y)
void setPixel(Image *img, int x, int y, Pixel *p)
{
    if (x >= 0 && x < img->width && y >= 0 && y < img->height)
    {
        img->pixels[y][x] = *p;
    }
}

// load a image (bmp, png, jpg..) and convert to custom struc Image to process
Image *loadImage(const char *file)
{
    SDL_Surface *img = IMG_Load(file);
    if (img == NULL)
    {
        printf("Failed to load the file\n");
        exit(EXIT_FAILURE);
    }

    Image *img_load = createImage(img->w, img->h);

    for (int y = 0; y < img->h; y++)
    {
        for (int x = 0; x < img->w; x++)
        {
            Uint32 *pixels = (Uint32 *)img->pixels;
            Uint32 pixel = pixels[y * img->w + x];

            Uint8 r, g, b;
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            Pixel p = {r, g, b};
            setPixel(img_load, x, y, &p);
            // printf("Pixel (%d,%d) = R:%d G:%d B:%d\n", x, y, r, g, b);
        }
    }
    return img_load;
}

// export bmp file of image
void exportImage(Image *img, const char *file)
{
    SDL_Surface *surf =
        SDL_CreateRGBSurfaceWithFormat(0,           // flags
                                       img->width,  // largeur
                                       img->height, // hauteur
                                       24, // profondeur (bits per pixel)
                                       SDL_PIXELFORMAT_RGB24 // format
        );

    if (!surf)
    {
        printf("Erreur création surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            Pixel *p = getPixel(img, x, y);
            Uint8 *pixel = (Uint8 *)surf->pixels + y * surf->pitch + x * 3;
            pixel[0] = p->r;
            pixel[1] = p->g;
            pixel[2] = p->b;
        }
    }

    SDL_SaveBMP(surf, file);
}

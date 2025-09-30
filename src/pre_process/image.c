#include "image.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

// use to alloc memory for a new image
Image *createImage(const int width, const int height)
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
Pixel *getPixel(const Image *img, const int x, const int y)
{
    if (!(x >= 0 && x < img->width && y >= 0 && y < img->height))
    {
        printf("Cannot read pixel (x:%d, y:%d)\n", x, y);
        exit(EXIT_FAILURE);
    }
    return &img->pixels[y][x];
}

// change tu pixel at (x, y)
void setPixel(const Image *img, const int x, const int y, const Pixel *p)
{
    if (x >= 0 && x < img->width && y >= 0 && y < img->height)
    {
        if(img->pixels != NULL)
            img->pixels[y][x] = *p;
    }
}

// create a new image from a image struct
Image *copyImage(const Image *img)
{
    Image *new = createImage(img->width, img->height);
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            Pixel *newP = getPixel(img, x, y);
            setPixel(new, x, y, newP);
        }
    }
    return new;
}

// load a image (bmp, png, jpg..) and convert to custom struc Image to process
Image *loadImage(const char *file)
{
    const SDL_Surface *img = IMG_Load(file);
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
            Uint8 *pixels = (Uint8 *)img->pixels;
            int bpp = img->format->BytesPerPixel;

            Uint8 *pPixel = pixels + y * img->pitch + x * bpp;

            Uint8 r, g, b;
            SDL_GetRGB(*(Uint32 *)pPixel, img->format, &r, &g, &b);
            Pixel p = {r, g, b};
            setPixel(img_load, x, y, &p);
            // printf("Pixel (%d,%d) = R:%d G:%d B:%d\n", x, y, r, g, b);
        }
    }
    return img_load;
}

// export bmp file of image
void exportImage(const Image *img, const char *file)
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

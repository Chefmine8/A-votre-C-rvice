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
        if (img->pixels != NULL)
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

SDL_Surface *getSDL_Surface(const Image *img)
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

    return surf;
}

// export bmp file of image
void exportImage(SDL_Surface *surf, const char *file)
{
    SDL_SaveBMP(surf, file);
}

// roate Image using SDL Texture
Image *rotateImage(const Image *src, double angle)
{
    if (!src)
        return NULL;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *win =
        SDL_CreateWindow("Hidden", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_HIDDEN);
    SDL_Renderer *renderer =
        SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *tex =
        SDL_CreateTextureFromSurface(renderer, getSDL_Surface(src));

    double rad = angle * M_PI / 180;

    int new_w = src->width * fabs(cos(rad)) + src->height * fabs(sin(rad));
    int new_h = src->width * fabs(sin(rad)) + src->height * fabs(cos(rad));

    SDL_Texture *target =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Point center = {src->width / 2, src->height / 2};
    SDL_Rect dstRect = {(new_w - src->width) / 2, (new_h - src->height) / 2,
                        src->width, src->height};
    SDL_RenderCopyEx(renderer, tex, NULL, &dstRect, angle, &center,
                     SDL_FLIP_NONE);

    SDL_Surface *resultSurf = SDL_CreateRGBSurfaceWithFormat(
        0, new_w, new_h, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888,
                         resultSurf->pixels, resultSurf->pitch);

    Image *result = createImage(new_w, new_h);

    for (int y = 0; y < new_h; y++)
    {
        for (int x = 0; x < new_w; x++)
        {
            Uint8 *pixels = (Uint8 *)resultSurf->pixels;
            int bpp = resultSurf->format->BytesPerPixel;

            Uint8 *pPixel = pixels + y * resultSurf->pitch + x * bpp;

            Uint8 r, g, b;
            SDL_GetRGB(*(Uint32 *)pPixel, resultSurf->format, &r, &g, &b);
            Pixel p = {r, g, b};
            setPixel(result, x, y, &p);
            // printf("Pixel (%d,%d) = R:%d G:%d B:%d\n", x, y, r, g, b);
        }
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(target);
    SDL_FreeSurface(resultSurf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return result;
}

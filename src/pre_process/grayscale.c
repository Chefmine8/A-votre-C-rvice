#include "grayscale.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// take custom Image struct amd process itch pixel to make it grey
void grayscaleImage(Image *img)
{
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            Pixel *p = getPixel(img, x, y);
            Uint8 gray = (Uint8)(p->r * 0.2125 + p->g * 0.7154 + p->b * 0.0721);
            p->r = gray;
            p->g = gray;
            p->b = gray;
            setPixel(img, x, y, p);
        }
    }
}



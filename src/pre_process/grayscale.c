#include "grayscale.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>

// take custom Image struct amd process itch pixel to make it grey
void grayscaleImage(const Image *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            Pixel *p = getPixel(img, x, y);
            const Uint8 gray = (Uint8) (p->r * 0.2125 + p->g * 0.7154 + p->b * 0.0721);
            p->r = gray;
            p->g = gray;
            p->b = gray;
            setPixel(img, x, y, p);
        }
    }
}

// Calculate the avarge grey level in a block of radius of a Image struc
double average(const Image *img, const int x, const int y, const int radius) {
    double sum = 0.0;
    const int count = (2 * radius + 1) * (2 * radius + 1);
    for (int i = x - radius; i <= x + radius; i++) {
        for (int j = y - radius; j <= y + radius; j++) {
            const Pixel *p = getPixel(img, i, j);
            sum += (double) (p->r);
        }
    }

    return sum / count;
}

// returm standard deviation (ecart type) of the grey level in a block of radius
// of a Image struct
double std_deviation(const Image *img, const int x, const int y, const int radius, const double avg) {
    double sum = 0.0;
    const int count = (2 * radius + 1) * (2 * radius + 1);
    for (int i = x - radius; i <= x + radius; i++) {
        for (int j = y - radius; j <= y + radius; j++) {
            const Pixel *p = getPixel(img, i, j);
            const double val = (double) (p->r);
            sum += (val - avg) * (val - avg);
        }
    }

    return sqrt(sum / count);
}

// sauvola algo (https://craftofcoding.wordpress.com/2021/10/06/thresholding-algorithms-sauvola-local/)
// n is radius for block size 
// R is dynamic range of standard deviation (default=128)
// k is constant value in range 0.2..0.5 (default = 0.5)
Image *sauvola(const Image *img, const int n, const int R, const float k) {
    const int w = img->width;
    const int h = img->height;

    Image *copy = copyImage(img);

    const int radius = (n - 1) / 2;

    for (int i = radius; i < w - radius; i++) {
        for (int j = radius; j < h - radius; j++) {
            // avarage
            const double m = average(img, i, j, radius);
            // standard deviation (ecart type)
            const double s = std_deviation(img, i, j, radius, m);

            // result
            const double threshold = m * (1 + k * ((s / R) - 1));

            Pixel *p = getPixel(copy, i, j);
            const double val = (double) (p->r);

            if (val < threshold) {
                p->r = 0;
                p->g = 0;
                p->b = 0;
                setPixel(copy, i, j, p);
            } else {
                p->r = 255;
                p->g = 255;
                p->b = 255;
                setPixel(copy, i, j, p);
            }
        }
    }

    return copy;
}

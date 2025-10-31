#pragma once

#include "../core/image.h"

/**
 * @brief Converts an image to grayscale.
 *
 * The conversion uses the standard luminance formula:
 * gray = 0.2125*R + 0.7154*G + 0.0721*B
 *
 * @param img Pointer to the Image to convert
 */
void grayscale_image(const Image *img);

/*<----------------------------->*/

/**
 * @brief Applies the Sauvola adaptive thresholding algorithm to an image.
 *
 * This algorithm performs local thresholding based on the mean and standard
 * deviation of pixels in a local neighborhood.
 *
 * @param img Pointer to the input Image (assumed grayscale)
 * @param n   Size of the neighborhood (block size, typically odd)
 * @param R   Dynamic range of standard deviation (default 128)
 * @param k   Sauvola constant (typically 0.2..0.5, default 0.5)
 * @return Image* Pointer to a new thresholded Image
 */
Image *sauvola(const Image *img, int n, int R, float k);

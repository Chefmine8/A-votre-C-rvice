#pragma once

#include <stdbool.h>

#include "../core/image.h"

/**
 * @brief Scales an image to the specified width scake and height using SDL for pixel manipulation.
 *
 * This function creates a new Image with the desired scale factor and uses SDL to handle the pixel data.
 * @param src     Pointer to the source Image
 * @param scale_x Scaling factor in the x direction (width)
 * @param scale_y Scaling factor in the y direction (height)
 * @return Image* Pointer to a newly allocated scaled Image
 */
Image *manual_image_scaling(const Image *src, float scale_x, float scale_y);


/**
 * @brief Resizes an image to a square shape by adding white padding as needed.
 *
 * The resulting image will have dimensions equal to the larger of the width or height of the source image.
 * The original image is centered within the new square image, and any extra space is filled with white pixels.
 *
 * @param src Pointer to the source Image
 * @return Image* Pointer to a newly allocated square Image
 */
Image *resize_image_square(const Image *src);

/**
 * @brief Resizes an image to the specified width and height, optionally maintaining the aspect ratio.
 *
 * If `keep_aspect_ratio` is true, the image is first resized to a square shape by adding white padding,
 * and then scaled to fit within the specified dimensions while preserving the original aspect ratio.
 * If false, the image is directly scaled to the specified width and height, which may distort the image.
 *
 * @param src                 Pointer to the source Image
 * @param width               Desired width of the output image
 * @param height              Desired height of the output image
 * @param keep_aspect_ratio   Boolean flag to indicate whether to maintain the aspect ratio
 * @return Image* Pointer to a newly allocated resized Image
 */
Image *resize_image(const Image *src, int width, int height, bool keep_aspect_ratio);


/**
 * @brief Prepares the input data for a neural network by resizing the image to 28x28 pixels
 *        and normalizing the pixel values.
 *
 * The function resizes the input image to 28x28 pixels, converts each pixel's red channel value
 * to a long double in the range [0, 1], and stores these values in a dynamically allocated array.
 * The caller is responsible for freeing the allocated memory.
 *
 * @param img Pointer to the input Image
 * @return long double* Pointer to an array of normalized pixel values (size 784)
 */
long double *get_nn_input(const Image *img);
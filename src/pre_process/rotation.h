#pragma once

#include "../core/image.h"

/**
 * @brief Rotates an image by a given angle (degrees) using SDL texture rendering.
 *
 * This function uses an off-screen SDL renderer and texture to rotate the image.
 * A new Image is created to store the rotated result.
 *
 * @param src   Pointer to the source Image
 * @param angle Rotation angle in degrees (clockwise)
 * @return Image* Pointer to a newly allocated rotated Image
 */
Image *manual_rotate_image(const Image *src, double angle);
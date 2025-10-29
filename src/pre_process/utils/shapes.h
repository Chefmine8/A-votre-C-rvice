#pragma once

#include "../../core/image.h"

/**
* @brief Pixel struct for shape
*/
typedef struct {
    int x, y; /**< x and y coord of the pixel */
    int isInShape; /**< boolean to check if a pixel is already in a sahpe */
    void *shape_ptr; /**< pointer to shape_struct */
} S_pixel;


/**
 * @brief Shape struct for filtering operations and detection
 *
 */
typedef struct {
    S_pixel *pixels; /**< dynamic list of pixels */
    int count; /**< number of pixel of a shape */
    int max_x, max_y, min_x, min_y; /**< bound of shape */
    int capacity; /**< capacity to dynamic malloc pixels */
} Shape;



/**
 * @brief Creates and allocate a new shape.
 *
 * @return shape* Pointer to the newly created shape
 */
 Shape* create_shape();



/**
 * @brief add a new pixel to the shape
 *
 * @param s     the shape we want add the pixel
 * @param x     x coord of the pixel
 * @param y     y coord of the pixel
 */
void shape_add_pixel(Shape *s, int x, int y);


/**
 * @brief free all pixel and shape
 *
 * @param s     the shape we want free
 */
void free_shape(Shape *s);


/**
 * @brief remove a specific shape from an image
 *
 * @param img   the pointer to the Image struct we want remove shape
 * @param s     the shape to remove
 */
void image_remove_shape(Image *img, Shape *s);
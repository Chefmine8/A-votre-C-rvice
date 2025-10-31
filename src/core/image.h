#pragma once

#include "../pre_process/utils/shapes.h"
#include <SDL2/SDL_image.h>
#include <stdint.h>

/**
 * @brief Pixel struct to store RGB values
 *
 */
typedef struct Pixel
{
    uint8_t r;        /**< Red component */
    uint8_t g;        /**< Green component */
    uint8_t b;        /**< Blue component */
    int x, y;         /**< coord */
    int isInShape;    /**< if is actually in shape */
    Shape *shape_ptr; /**< pointer to the shape or NULL if not */
} Pixel;

/**
 * @brief Image struct to store image data.
 * Use pointer to pointer to create a 2D array of pixels struct
 */
typedef struct Image
{
    int width, height; /**< Width and height of the image */
    Pixel **pixels;    /**< 2D array of pixels [height][width] */
} Image;

// func
/**
 * @brief Creates a new image with the specified width and height.
 *
 * @param width  Image width
 * @param height Image height
 * @return Image* Pointer to the newly created image
 */
Image *create_image(int width, int height);

/*<----------------------------->*/

/**
 * @brief Frees the memory allocated for an image.
 *
 * @param img Pointer to the image to free
 */
void free_image(Image *img);

/*<----------------------------->*/

/**
 * @brief Retrieves the pixel at position (x, y) in the image.
 *
 * @param img Pointer to the image
 * @param x   X coordinate
 * @param y   Y coordinate
 * @return Pixel* Pointer to the pixel
 */
Pixel *get_pixel(const Image *img, int x, int y);

/*<----------------------------->*/

/**
 * @brief Sets the pixel at position (x, y) in the image.
 *
 * @param img Pointer to the image
 * @param x   X coordinate
 * @param y   Y coordinate
 * @param p   Pointer to the pixel to copy
 */
void set_pixel(const Image *img, int x, int y, const Pixel *p);

/*<----------------------------->*/

/**
 * @brief change the pixel color at position (x, y) in the image.
 *
 * @param img Pointer to the image
 * @param x   X coordinate
 * @param y   Y coordinate
 * @param r,g,b color (0-255)
 */
void set_pixel_color(const Image *img, int x, int y, uint8_t r, uint8_t g,
                     uint8_t b);

/*<----------------------------->*/

/**
 * @brief Creates a copy of an existing image.
 *
 * @param img Pointer to the source image
 * @return Image* Pointer to the copied image
 */
Image *copy_image(const Image *img);

/*<----------------------------->*/

/**
 * @brief Converts an SDL_Surface to our custom Image structure.
 *
 * @param surf Pointer to the SDL_Surface
 * @param img  Pointer to an existing Image (NULL to create a new one)
 * @return Image* Pointer to the resulting Image (can be ignored if img is not
 * NULL)
 */
Image *sdl_surface_to_image(const SDL_Surface *surf, Image *img);

/*<----------------------------->*/

/**
 * @brief Converts a custom Image structure to an SDL_Surface.
 *
 * @param img Pointer to the Image
 * @return SDL_Surface* Pointer to the created SDL_Surface
 */
SDL_Surface *image_to_sdl_surface(const Image *img);

/*<----------------------------->*/

/**
 * @brief Loads an image from a file and converts it to our Image structure.
 *
 * @param file File path
 * @return Image* Pointer to the loaded image
 */

/*<----------------------------->*/

Image *load_image(const char *file);

/**
 * @brief Exports an SDL_Surface as a BMP file.
 *
 * @param surf Pointer to the SDL_Surface
 * @param file Output file path
 */
void export_image(SDL_Surface *surf, const char *file);

/**
 * @brief Draws a line on the image using Bresenham's line algorithm.
 *
 * @param img Pointer to the image
 * @param x0  Starting x coordinate
 * @param y0  Starting y coordinate
 * @param x1  Ending x coordinate
 * @param y1  Ending y coordinate
 * @param r   Red component of the line color
 * @param g   Green component of the line color
 * @param b   Blue component of the line color
 */
void draw_line(Image *img, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g,
               uint8_t b);

/**
 * @brief Extracts a sub-image from the given image.
 *
 * @param img      Pointer to the source image
 * @param x_start  Starting x coordinate
 * @param y_start  Starting y coordinate
 * @param x_end    Ending x coordinate
 * @param y_end    Ending y coordinate
 * @return Image*  Pointer to the extracted sub-image
 */
Image *extract_sub_image(const Image *img, int x_start, int y_start, int x_end,
                         int y_end);

#pragma once

#include <SDL2/SDL_image.h>
#include <stdint.h>

/**
 * @brief Pixel struct to store RGB values
 *
 */
typedef struct
{
    uint8_t r; /**< Red component */
    uint8_t g; /**< Green component */
    uint8_t b; /**< Blue component */
} Pixel;

/**
 * @brief Image struct to store image data.
 * Use pointer to pointer to create a 2D array of pixels struct
 */
typedef struct
{
    int width, height; /**< Width and height of the image */
    Pixel **pixels; /**< 2D array of pixels [height][width] */
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
 * @return Image* Pointer to the resulting Image (can be ignored if img is not NULL)
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


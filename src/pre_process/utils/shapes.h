#pragma once

typedef struct Shape Shape;
typedef struct Image Image;
typedef struct Pixel Pixel;


/**
 * @brief Shape struct for filtering operations and detection
 *
 */
struct Shape{
    Pixel **pixels; /**< dynamic list of pixels */
    int count; /**< number of pixel of a shape */
    int max_x, max_y, min_x, min_y; /**< bound of shape */
    int capacity; /**< capacity to dynamic malloc pixels */
};



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
 * @param p     the pointer of the Shpixel to add
 */
void shape_add_pixel(Shape *s, Pixel *p);

/**
 * @brief set all pixel of a shape in a different color in the original image
 *
 * @param img   the pointer to the Image struct we want recolor shape
 * @param s     the shape to recolor
 * @param r     red (0-255)
 * @param g     green (0-255)
 * @param b     blue (0-255)
 */
void image_change_shape_color(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b);


/**
 * @brief remove a specific shape from an image
 *
 * @param img   the pointer to the Image struct we want remove shape
 * @param s     the shape to remove
 */
void image_remove_shape(Image *img, Shape *s);

/**
 * @brief add existing shape to an Image
 *
 * @param img   the pointer to the Image struct we want add the shape
 * @param s     the shape to add
 * @param r,g,b color (0-255)
 */
void image_add_shape(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief free all pixel and shape
 *
 * @param s     the shape we want free
 */



/**
 * @param s     the shape
 * @return return the shape width it is the max distance betwin min_x and max_x
 */
int shape_width(Shape *s);


/**
 * @param s     the shape
 * @return return the shape height it is the max distance betwin min_y and max_y
 */
int shape_height(Shape *s);


/**
 * @param s     the shape
 * @return return area of the shape, width * height
 */
int shape_area(Shape *s);


/**
 * @param s     the shape
 * @return return aspect ratio of the shape consider at a rectangle
 */
double shape_aspect_ratio(Shape *s);

/**
 * @param s     the shape
 * @return return density of the shape consider at a rectangle (proportion of black pixel)
 */
double shape_density(Shape *s);


void free_shape(Shape *s);


// --- shape detection --- //

Shape **get_all_shape(Image* img);
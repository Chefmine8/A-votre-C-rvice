#pragma once

#include <limits.h>
#include <stdint.h>

/* Forward declarations */
typedef struct Shape Shape;
typedef struct Image Image;
typedef struct Pixel Pixel;
typedef struct
{
    int x, y;
} Coord;

/**
 * @brief Represents a connected region (shape) in an image.
 *
 * Each Shape contains a dynamically allocated list of pixel pointers
 * that belong to the shape, as well as its geometric boundaries and state.
 */
struct Shape
{
    Pixel **pixels; /**< Dynamic list of pointers to pixels belonging to the
                       shape. */
    int count;      /**< Number of pixels in the shape. */
    int max_x, max_y, min_x,
        min_y;            /**< Bounding box coordinates of the shape. */
    int capacity;         /**< Current allocated capacity for pixels. */
    int has_been_removed; /**< Flag indicating whether the shape has been
                             removed from the image. */
};

/**
 * @brief Creates and allocates a new empty shape.
 *
 * Initializes internal structures and boundary values.
 *
 * @return Pointer to the newly allocated Shape.
 */
Shape *create_shape(void);

/**
 * @brief Adds a pixel to the specified shape.
 *
 * Expands the shape's pixel list dynamically if needed and updates its
 * boundaries.
 *
 * @param s Pointer to the shape to modify.
 * @param p Pointer to the pixel to add.
 */
void shape_add_pixel(Shape *s, Pixel *p);

/**
 * @brief Recolors all pixels belonging to a shape in the given image.
 *
 * @param img Pointer to the Image structure.
 * @param s Pointer to the shape to recolor.
 * @param r Red component (0–255).
 * @param g Green component (0–255).
 * @param b Blue component (0–255).
 */
void image_change_shape_color(Image *img, Shape *s, uint8_t r, uint8_t g,
                              uint8_t b);

/**
 * @brief Removes a specific shape from an image by setting its pixels to white.
 *
 * @param img Pointer to the Image structure.
 * @param s Pointer to the shape to remove.
 */
void image_remove_shape(Image *img, Shape *s);

/**
 * @brief Adds an existing shape to an image with a specific color.
 *
 * @param img Pointer to the Image structure.
 * @param s Pointer to the shape to draw.
 * @param r Red component (0–255).
 * @param g Green component (0–255).
 * @param b Blue component (0–255).
 */
void image_add_shape(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief free all pixel and shape
 *
 * @param s     the shape we want free
 */

/**
 * @brief Computes the width of a shape.
 *
 * @param s Pointer to the shape.
 * @return The width (max_x - min_x + 1).
 */
int shape_width(Shape *s);

/**
 * @brief Computes the height of a shape.
 *
 * @param s Pointer to the shape.
 * @return The height (max_y - min_y + 1).
 */
int shape_height(Shape *s);

/**
 * @brief Computes the bounding box area of a shape.
 *
 * @param s Pointer to the shape.
 * @return The area in pixels (width × height).
 */
int shape_area(Shape *s);

/**
 * @brief Computes the aspect ratio (width/height) of a shape.
 *
 * @param s Pointer to the shape.
 * @return The aspect ratio as a double. Returns 0 if height is zero.
 */
double shape_aspect_ratio(Shape *s);

/**
 * @brief Computes the density of a shape within its bounding box.
 *
 * Density = (number of pixels in shape) / (area of bounding box)
 *
 * @param s Pointer to the shape.
 * @return The density value between 0 and 1.
 */
double shape_density(Shape *s);

/**
 * @brief Detects and extracts all shapes from an image.
 *
 * A shape is defined as a connected group of dark pixels (r < 128).
 *
 * @param img Pointer to the Image structure to analyze.
 * @return A NULL-terminated array of pointers to Shape structures.
 */

/**
 * @brief Frees the memory allocated for a shape and its pixel list.
 *
 * @param s Pointer to the shape to free.
 */
void free_shape(Shape *s);

// --- shape detection --- //

/**
 * @brief Detects and extracts all shapes from an image.
 *
 * A shape is defined as a connected group of dark pixels (r < 128).
 *
 * @param img Pointer to the Image structure to analyze.
 * @return A NULL-terminated array of pointers to Shape structures.
 */
Shape **get_all_shape(Image *img);

/**
 * @brief Removes shapes marked as removed from the shape list.
 *
 * @param shapes NULL-terminated array of shape pointers.
 */
void clean_shapes(Shape **shapes);

/**
 * @brief Removes small shapes from the image based on pixel count.
 *
 * Shapes with fewer pixels than the threshold are removed.
 *
 * @param img Pointer to the Image structure.
 * @param shapes NULL-terminated array of shape pointers.
 * @param threshold Minimum pixel count to keep a shape.
 */
void remove_small_shape(Image *img, Shape **shapes, int threshold);

/**
 * @brief Removes outlier shapes based on area.
 *
 * Outliers are identified using percentiles, IQR, and mean-based thresholds.
 *
 * @param img Pointer to the Image structure.
 * @param shapes NULL-terminated array of shape pointers.
 * @param low_percentile Lower percentile for area filtering (e.g., 25).
 * @param high_percentile Upper percentile for area filtering (e.g., 75).
 * @param iqr_factor Multiplier for the interquartile range threshold.
 * @param mean_factor Multiplier for the mean area threshold.
 */
void remove_outliers_shape(Image *img, Shape **shapes, int low_percentile,
                           int high_percentile, double iqr_factor,
                           double mean_factor);

/**
 * @brief Removes shapes whose aspect ratios fall outside the given range.
 *
 * @param img Pointer to the Image structure.
 * @param shapes NULL-terminated array of shape pointers.
 * @param min_ration Minimum aspect ratio allowed.
 * @param max_ratio Maximum aspect ratio allowed.
 */
void remove_aspect_ration(Image *img, Shape **shapes, double min_ration,
                          double max_ratio);

/**
 * @brief Merges shapes that are within a specified spacing of each other.
 *
 * @param shapes NULL-terminated array of shape pointers.
 * @param spacing Maximum distance between shapes to consider for merging.
 */
void merge_shapes(Shape **shapes, int spacing);

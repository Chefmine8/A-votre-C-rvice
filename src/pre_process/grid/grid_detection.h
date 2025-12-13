#pragma once

#include "../../core/image.h"
#include "../utils/shapes.h"
#include <stdint.h>

/**
 * @brief Converts degrees to radians.
 *
 * @param degrees Angle in degrees.
 * @return Angle in radians.
 */

/**
 * @brief Generates an image where each detected shape is replaced by a filled
 * circle.
 *
 * The circle is centered on the shape's centroid and scaled by its pixel count.
 * Useful for preprocessing before applying a Hough transform.
 *
 * @param img Pointer to the input Image structure.
 * @param shapes NULL-terminated array of detected Shape pointers.
 * @param scale_factor Scaling factor applied to the circle radius.
 * @return Pointer to a new Image containing the circularized shapes.
 */
Image *circle_image(Image *img, Shape **shapes, double scale_factor);

/**
 * @brief Computes the Hough transform accumulator space for line detection.
 *
 * Each dark pixel (r < 128) in the image contributes to the accumulator
 * for all possible (rho, theta) values.
 *
 * @param img Pointer to the input binary or grayscale image.
 * @param theta_range Output parameter — set to the number of theta steps used
 * (typically 180).
 * @param rho_max Output parameter — set to the maximum rho distance in pixels.
 * @return 2D array (theta × 2*rho_max) representing the Hough accumulator.
 *
 * @note Memory must be released using @ref free_hough().
 */
int **hough_space(Image *img, int *theta_range, int *rho_max);

/**
 * @brief Filters the Hough space based on a relative threshold.
 *
 * Values below a fraction of the maximum accumulator value are set to zero.
 *
 * @param hough_space 2D array of accumulator values.
 * @param theta_range Number of theta bins (usually 180).
 * @param rho_max Maximum rho distance.
 * @param threshold_ratio Ratio (0–1) of the maximum value to use as the cutoff
 * threshold.
 */
void hough_space_filter(int **hough_space, int theta_range, int rho_max,
                        double threshold_ratio);

/**
 * @brief Frees the memory allocated for a Hough transform accumulator.
 *
 * @param hough_space Pointer to the Hough space 2D array.
 * @param theta_range Number of theta bins used.
 * @param rho_max Maximum rho distance.
 */
void free_hough(int **hough_space, int theta_range);

/**
 * @brief Filters the Hough space to retain only vertical and horizontal lines.
 *
 * Removes noisy or diagonal line detections and merges nearby line peaks
 * that correspond to the same grid line.
 *
 * @param hough_space 2D array of accumulator values.
 * @param theta_range Number of theta bins (typically 180).
 *
 * @details
 * - Lines close to 0° and 90° are preserved.
 * - Nearby rho/theta cells are merged to reduce duplicates.
 */
void filter_line(int **hough_space, int theta_range, int rho_max,
                 int theta_prox, int rho_prox);
/**
 * @brief Draws detected lines from a Hough accumulator onto an image.
 *
 * Each non-zero cell in the Hough space corresponds to a line defined by:
 *     ρ = x·cos(θ) + y·sin(θ)
 *
 * The function computes line endpoints within image bounds and draws them in
 * red.
 *
 * @param image Pointer to the Image structure where lines will be drawn.
 * @param hough_space 2D array of accumulator values.
 * @param theta_range Number of theta bins (typically 180).
 * @param rho_max Maximum rho distance.
 * @param r,g,b Color components for the drawn lines (0-255).
 */
void draw_lines(Image *image, int **hough_space, int theta_range, int rho_max,
                int r, int g, int b);

/**
 * @brief Extracts horizontal lines from the Hough space.
 *
 * @param hough_space 2D array of accumulator values.
 * @param theta_range Number of theta bins (typically 180).
 * @param rho_max Maximum rho distance.
 * @param delta error margin for line detection.
 * @return 2D array newly allocated containing the horizontal lines.
 */
int **horizontal_lines(int **hough_space, int theta_range, int rho_max,
                       int delta);

/**
 * @brief Extracts vertical lines from the Hough space.
 *
 * @param hough_space 2D array of accumulator values.
 * @param theta_range Number of theta bins (typically 180).
 * @param rho_max Maximum rho distance.
 * @param delta error margin for line detection.
 * @return 2D array newly allocated containing the vertical lines.
 */
int **vertical_lines(int **hough_space, int theta_range, int rho_max,
                     int delta);

/**
 * @brief Filters lines in the Hough space using gaps.
 *
 * This function calculates best subsets of lines with regular spacing and using
 * magic
 *
 * @param hough_space 2D array of accumulator values.
 * @param theta_range Number of theta bins (typically 180).
 * @param rho_max Maximum rho distance.
 */
void filter_gaps(int **hough_space, int theta_range, int rho_max);

/**
 * @brief Computes the bounding box of the detected grid lines.
 *
 *
 * @param vertical_lines 2D array of vertical lines from Hough space.
 * @param horizontal_lines 2D array of horizontal lines from Hough space.
 * @param theta_range Number of theta bins (typically 180).
 * @param rho_max Maximum rho distance.
 * @param x_start Output parameter for the starting x-coordinate of the bounding
 * box.
 * @param x_end Output parameter for the ending x-coordinate of the bounding
 * box.
 * @param y_start Output parameter for the starting y-coordinate of the bounding
 * box.
 * @param y_end Output parameter for the ending y-coordinate of the bounding
 * box.
 */
void get_bounding_box(int **vertical_lines, int **horizontal_lines,
                      int theta_range, int rho_max, int *x_start, int *x_end,
                      int *y_start, int *y_end);

/**
 * @brief Filters shapes based on the number of neighboring shapes.
 *
 * Shapes with fewer than `min_neighbors` aligned neighbors
 * (horizontally or vertically) are removed from the image and marked as
 * removed.
 *
 * @param img Pointer to the Image structure containing the shapes.
 * @param shapes NULL-terminated array of detected Shape pointers.
 * @param min_neighbors Minimum number of aligned neighboring shapes required to
 * retain a shape.
 */
void filter_by_density(Image *img, Shape **shapes, int min_neighbors);

// Same as above but only in one direction
void filter_by_density_h(Image *img,Shape **shapes, int min_neighbors);

// Same as above but only in one direction
void filter_by_density_v(Image *img,Shape **shapes, int min_neighbors);

/** * @brief Detects the grid size (number of rows and columns) from the shapes.
 *
 * @param shapes NULL-terminated array of detected Shape pointers.
 * @param rows Output parameter for the number of rows detected.
 * @param cols Output parameter for the number of columns detected.
 */
void detect_grid_size(Shape **shapes, int *rows, int *cols);

/** * @brief Extracts individual grid cells from the image based on detected
 * shapes.
 *
 * @param img Pointer to the input Image structure.
 * @param shapes NULL-terminated array of detected Shape pointers.
 * @param rows Number of rows in the grid.
 * @param cols Number of columns in the grid.
 * @return 2D array of Image pointers representing the extracted grid cells.
 */
Image ***get_grid_cells(Image *img, Shape **shapes, int rows, int cols, char *path);
/** * @brief Retrieves all world from the world liste using the shapes.
 *  * use this function on a image without the grid present
 * @param shapes NULL-terminated array of detected Shape pointers.
 * @return 2D array of Shape pointers representing all world.
 */
Shape ***get_all_world(Shape **shapes);
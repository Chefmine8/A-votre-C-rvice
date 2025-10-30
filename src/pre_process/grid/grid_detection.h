#pragma once

#include <stdint.h>
#include "../utils/shapes.h"
#include "../../core/image.h"

Image *circle_image(Image *img, Shape** shapes, double scale_factor);

int **hough_space(Image *img, int* theta_range, int* rho_max);

void hough_space_filter(int **hough_space,int theta_range, int rho_max, double threshold_ratio);

void free_hough(int **hough_space,int theta_range, int rho_max );

void filter_line(int **hough_space,int theta_range, int rho_max);


void draw_lines(Image *image, int** hough_space,int theta_range, int rho_max);
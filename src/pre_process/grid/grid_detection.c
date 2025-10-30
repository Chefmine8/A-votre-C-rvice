#include "grid_detection.h"


Image *circle_image(Image *img, Shape** shapes, double scale_factor)
{
    Image *res = create_image(img->width, img->height);

    for (int i = 0; shapes[i] != NULL; ++i)
    {
        if(shapes[i]->has_been_removed != 0)
            continue;
        int cx = (int)((shapes[i]->min_x + shapes[i]->max_x) / 2);
        int cy = (int)((shapes[i]->min_y + shapes[i]->max_y) / 2);

        int val1 = (int)(sqrt(shapes[i]->count) * scale_factor);
        int radius = val1 > 1 ? val1 : 1;

        for (int y = cy - radius; y < cy + radius + 1; ++y)
        {
            for (int x = cx - radius; x < cx + radius + 1; x++)
            {
                if (x >= 0 && y >= 0 && x < res->width && y < res->height)
                {
                    if(pow(x - cx, 2) + pow(y - cy, 2) <= pow(radius, 2))
                    {
                        set_pixel_color(res, x, y, 0,0,0);
                    }
                }
            }
        }
    }
    return res;
}

// Convert degrees to radians
double deg2rad(double degrees) {
    return degrees * M_PI / 180.0;
}

int **hough_space(Image *img, int* theta_range, int* rho_max)
{
    *theta_range = 180;
    *rho_max = (int)(sqrt(img->width * img->width + img->height * img->height));
    int **hough_space = malloc(*theta_range * sizeof(int*));
    if (!hough_space)
    {
        printf("Cannot allocate memory for the hough space\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < *theta_range; i++)
    {
        hough_space[i] = calloc(2 * (*rho_max), sizeof(int));
        if (!hough_space[i])
        {
            printf("Cannot allocate memory for the hough space\n");
            exit(EXIT_FAILURE);
        }
    }

    // Fill the Hough space
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; ++x)
        {
            if (get_pixel(img, x, y)->r < 128)
            {
                for (int theta = 0; theta < *theta_range; theta ++)
                {
                    int rho = (int)round(x * cos(deg2rad(theta)) + y * sin(deg2rad(theta))) + *rho_max;
                    hough_space[theta][rho] +=1;
                }
            }
        }
    }
    return hough_space;
}

void hough_space_filter(int **hough_space,int theta_range, int rho_max, double threshold_ratio)
{
    int max = 0;
    for (int i = 0; i < theta_range; i++) {
        for (int j = 0; j < 2 * rho_max; j++) {
            if (hough_space[i][j] > max)
                max = hough_space[i][j];
        }
    }

    int threshold = (int)(threshold_ratio * max);
    for (int i = 0; i < theta_range; i++) {
        for (int j = 0; j < 2 * rho_max; j++) {
            if (hough_space[i][j] < threshold)
                hough_space[i][j] = 0;
        }
    }

}


void filter_line(int **hough_space,int theta_range, int rho_max, int theta_prox, int rho_prox) {
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if ((theta > 10 && theta < 80) || (theta > 100 && theta < 170))
                hough_space[theta][rho] = 0;
        }
    }

    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            int val = hough_space[theta][rho];
            if (val == 0)
                continue;

            int sum_rho = 0;
            int count = 0;

            int best_theta = theta;
            double best_theta_diff = 9999.0;

            for (int dt = -theta_prox; dt <= theta_prox; dt++) {
                for (int dr = -rho_prox; dr <= rho_prox; dr++) {
                    int nt = theta + dt;
                    int nr = rho + dr;

                    if (nt < 0 || nt >= theta_range || nr < 0 || nr >= 2 * rho_max)
                        continue;

                    if (hough_space[nt][nr] > 0) {
                        sum_rho += nr;
                        count++;

                        double t = nt;
                        double diff = fmin(fabs(t - 0), fmin(fabs(t - 90), fabs(t - 180)));

                        if (diff < best_theta_diff) {
                            best_theta_diff = diff;
                            best_theta = nt;
                        }

                        if (!(nt == theta && nr == rho))
                            hough_space[nt][nr] = 0;
                    }
                }
            }

            if (count > 0) {
                int avg_rho = sum_rho / count;
                hough_space[best_theta][avg_rho] = val;
            }

            if (!(best_theta == theta && (sum_rho / count) == rho))
                hough_space[theta][rho] = 0;

            if (theta != 90)
            {
                if (theta !=0)
                    hough_space[theta][rho] = 0;
            }
            if (theta != 0)
            {
                if (theta !=90)
                    hough_space[theta][rho] = 0;
            }
        }
    }
}


void draw_lines(Image *image, int** hough_space,int theta_range, int rho_max)
{
    for (int i = 0; i < theta_range; i++) {
        for (int j = 0; j < 2 * rho_max; j++) {
            if (hough_space[i][j] > 0) {
                int theta = i;
                int rho = j - rho_max;

                double t = deg2rad(theta);
                int points[4][2];
                int count = 0;

                // left
                if (fabs(sin(t)) > 1e-6) {
                    int y = (int)(rho / sin(t));
                    if (y >= 0 && y < image->height) {
                        points[count][0] = 0;
                        points[count][1] = y;
                        count++;
                    }
                }

                // right
                if (fabs(sin(t)) > 1e-6) {
                    int y = (int)((rho - (image->width - 1) * cos(t)) / sin(t));
                    if (y >= 0 && y < image->height) {
                        points[count][0] = image->width - 1;
                        points[count][1] = y;
                        count++;
                    }
                }

                // up
                if (fabs(cos(t)) > 1e-6) {
                    int x = (int)(rho / cos(t));
                    if (x >= 0 && x < image->width) {
                        points[count][0] = x;
                        points[count][1] = 0;
                        count++;
                    }
                }

                // down
                if (fabs(cos(t)) > 1e-6) {
                    int x = (int)((rho - (image->height - 1) * sin(t)) / cos(t));
                    if (x >= 0 && x < image->width) {
                        points[count][0] = x;
                        points[count][1] = image->height - 1;
                        count++;
                    }
                }

                // drw line
                if (count >= 2) {
                    int x1 = points[0][0], y1 = points[0][1];
                    int x2 = points[1][0], y2 = points[1][1];

                    draw_line(image, x1, y1, x2, y2, 255, 0, 0); // ligne rouge
                }
            }
        }
    }
}

void free_hough(int **hough_space,int theta_range, int rho_max )
{
    for (int i = 0; i < theta_range; ++i)
    {
        free(hough_space[i]);
    }
    free(hough_space);
}
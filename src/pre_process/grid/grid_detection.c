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

int **horizontal_lines(int **hough_space,int theta_range, int rho_max, int delta)
{
    int **res = malloc(theta_range * sizeof(int*));
    if (!res)
    {
        printf("Cannot allocate memory for the horizontal lines\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < theta_range; i++)
    {
        res[i] = calloc(2 * rho_max, sizeof(int));
        if (!res[i])
        {
            printf("Cannot allocate memory for the horizontal lines\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < theta_range; i++) {
        for (int j = 0; j < 2 * rho_max; j++) {
            if (hough_space[i][j] > 0) {
                if ((i >= 90 -delta && i <= 90 + delta)) {
                    res[i][j] = hough_space[i][j];
                }
            }
        }
    }
    return res;
}

int **vertical_lines(int **hough_space,int theta_range, int rho_max, int delta)
{
    int **res = malloc(theta_range * sizeof(int*));
    if (!res)
    {
        printf("Cannot allocate memory for the horizontal lines\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < theta_range; i++)
    {
        res[i] = calloc(2 * rho_max, sizeof(int));
        if (!res[i])
        {
            printf("Cannot allocate memory for the horizontal lines\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < theta_range; i++) {
        for (int j = 0; j < 2 * rho_max; j++) {
            if (hough_space[i][j] > 0) {
                if ((i >= 0 -delta && i <= 0 + delta) || (i >= 180 -delta && i <= 180 + delta)) {
                    res[i][j] = hough_space[i][j];
                }
            }
        }
    }
    return res;
}

void draw_lines(Image *image, int** hough_space,int theta_range, int rho_max, int r, int g, int b)
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

                    draw_line(image, x1, y1, x2, y2, r, g, b);
                }
            }
        }
    }
}

void free_hough(int **hough_space,int theta_range)
{
    for (int i = 0; i < theta_range; ++i)
    {
        free(hough_space[i]);
    }
    free(hough_space);
}

int comp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void filter_gaps(int **hough_space, int theta_range, int rho_max)
{
    //printf("\n\n<----------------------->\n\n");
    int count = 0;
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if(hough_space[theta][rho] > 0)
            {
                count++;
            }
        }
    }

    if (count < 3) return;

    int *list_rho = malloc(count * sizeof(int));
    int i = 0;
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if(hough_space[theta][rho] > 0)
            {
                list_rho[i] = rho;
                i++;
            }
        }
    }
    qsort(list_rho, count, sizeof(int), comp);

    int *gaps = malloc((count - 1) * sizeof(int));
    for (int j = 1; j < count; j++) {
        gaps[j - 1] = list_rho[j] - list_rho[j - 1];
    }

    double mean =0.0;
    for (int j = 0; j < count - 1; j++) {
        mean += gaps[j];
    }
    mean /= (count - 1);

    double stddev =0.0;
    for (int j = 0; j < count - 1; j++) {
        stddev += pow(gaps[j] - mean, 2);
    }
    stddev = sqrt(stddev / (count - 1));

    double threshold = stddev * 1.5;

    // find best subset
    int start_index = 0, best_start = 0, best_len = 0;
    for (int j = 1; j < count -1; j++) {
        if (fabs(gaps[j] - mean) > threshold * 2) {
            int len = j - start_index;
            if (len > best_len) {
                best_len = len;
                best_start = start_index;
            }
            start_index = j;
        }
    }
    if ((count -1 - start_index) > best_len) {
        best_len = count -1 - start_index;
        best_start = start_index;
    }

    // Zero out rhos not in the best subset
    for (int j = 0; j < count; j++) {
        if (j < best_start || j > best_start + best_len) {
            int rho_to_remove = list_rho[j];
            for (int theta = 0; theta < theta_range; theta++) {
                hough_space[theta][rho_to_remove] = 0;
            }
        }
    }


    int count_2 = 0;
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if(hough_space[theta][rho] > 0)
            {
                count_2++;
            }
        }
    }


    int *list_rho_2 = malloc(count_2 * sizeof(int));
    int i_2 = 0;
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if(hough_space[theta][rho] > 0)
            {
                list_rho_2[i_2] = rho;
                i_2++;
            }
        }
    }
    qsort(list_rho_2, count_2, sizeof(int), comp);

    int *gaps_2 = malloc((count_2 - 1) * sizeof(int));
    for (int j = 1; j < count_2; j++) {
        gaps_2[j - 1] = list_rho_2[j] - list_rho_2[j - 1];
        //printf("Gap %d: %d | Rho1: %d | Rho2: %d\n", j - 1, gaps_2[j - 1], list_rho_2[j - 1], list_rho_2[j]);
    }

    int* indexes_to_remove = malloc((count_2 -1) * sizeof(int));

    double mean_2 =0.0;
    for (int j = 0; j < count_2 - 1; j++) {
        mean_2 += gaps_2[j];
    }
    mean_2 /= (count_2 - 1);

    double stddev_2 = 0.0;
    for (int j = 0; j < count_2 - 1; j++) {
        stddev_2 += pow(gaps_2[j] - mean_2, 2);
    }
    stddev_2 = sqrt(stddev_2 / (count_2 - 1));

    double threshold_2 = mean_2 + 2 * stddev_2 + 10;

    //printf("<-> threshold_2: %f | mean_2: %f\n", threshold_2, mean_2);

    for (int j = 0; j < count_2 - 1; j++) {
        //printf("Gap %d: %d | Mean_2: %f | Threshold_2: %f\n", j, gaps_2[j], mean_2, threshold_2);
        if (gaps_2[j] > threshold_2) {
            int rho_to_remove;
            if(j == 0)
                rho_to_remove = list_rho_2[j];
            else if (j == count_2 -2)
                rho_to_remove = list_rho_2[j +1];
            else
            {
                int gap_before = gaps_2[j -1];
                int gap_after = gaps_2[j +1];
                if (gap_before < gap_after)
                    rho_to_remove = list_rho_2[j];
                else
                    rho_to_remove = list_rho_2[j +1];
            }

            for (int theta = 0; theta < theta_range; theta++) {
                hough_space[theta][rho_to_remove] = 0;
            }
        }
    }

    free(indexes_to_remove);
    free(list_rho_2);
    free(gaps_2);

    free(gaps);
    free(list_rho);
}

void get_bounding_box(int **vertical_lines, int** horizontal_lines,  int theta_range, int rho_max, int *x_start, int *x_end, int *y_start, int *y_end)
{
    int h_count =0, v_count =0;
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if(vertical_lines[theta][rho] > 0)
                v_count++;

            if (horizontal_lines[theta][rho] > 0)
                h_count++;

        }
    }

    int *list_rho_h = malloc(h_count * sizeof(int));
    int *list_rho_v = malloc(v_count * sizeof(int));
    int i_h = 0, i_v = 0;
    for (int theta = 0; theta < theta_range; theta++) {
        for (int rho = 0; rho < 2 * rho_max; rho++) {
            if(horizontal_lines[theta][rho] > 0)
                list_rho_h[i_h++] = rho;
            if(vertical_lines[theta][rho] > 0)
                list_rho_v[i_v++] = rho;
        }
    }

    *x_start = list_rho_v[0] - rho_max;
    *x_end = list_rho_v[v_count -1] - rho_max;
    *y_start = list_rho_h[0] - rho_max;
    *y_end = list_rho_h[h_count -1] - rho_max;

//    for (int i = 0; i < v_count; i++) {
//        int rho_v = list_rho_v[i] - rho_max;
//        int x = rho_v;
//        for (int j = 0; j < h_count; j++) {
//            int rho_h = list_rho_h[j] - rho_max;
//            int y = rho_h;
//
//            // x,y point of intersection
//            printf("Intersection: (%d, %d)\n", x, y);
//            //set_pixel_color(img, x, y, 0, 255, 0);
//        }
//    }

    //set_pixel_color(img, 10, 10, 0,255,0);
    free(list_rho_h);
    free(list_rho_v);

}
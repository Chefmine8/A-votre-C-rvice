#include "grid_detection.h"

static int compare_shape_y(const void *a, const void *b)
{
    const Shape *s1 = *(const Shape **)a;
    const Shape *s2 = *(const Shape **)b;
    return s1->min_y - s2->min_y;
}

static int compare_shape_x(const void *a, const void *b)
{
    const Shape *s1 = *(const Shape **)a;
    const Shape *s2 = *(const Shape **)b;
    return s1->min_x - s2->min_x;
}

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

    //check if no lines detected
    if (h_count ==0 || v_count ==0)
    {
        printf("No lines detected for bounding box calculation\n");
        return;
        exit(EXIT_FAILURE);
    }

    int *list_rho_h = malloc(h_count * sizeof(int));
    if (list_rho_h == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    int *list_rho_v = malloc(v_count * sizeof(int));
    if (list_rho_v == NULL) {
            printf("Memory allocation failed\n");
            exit(EXIT_FAILURE);
    }
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

void filter_by_density(Image *img,Shape **shapes, int min_neighbors)
{
    if (shapes == NULL)
        return;

    int count = 0;
    while (shapes[count] != NULL)
        count++;

    for (int i = 0; i < count; i++)
    {
        Shape *s = shapes[i];
        if (s->has_been_removed != 0)
            continue;

        int h_count = 0, v_count = 0;

        int x_min = s->min_x;
        int x_max = s->max_x;
        int y_min = s->min_y;
        int y_max = s->max_y;

        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            Shape *other = shapes[j];
            if (other->has_been_removed != 0)
                continue;

            int other_x_min = other->min_x;
            int other_x_max = other->max_x;
            int other_y_min = other->min_y;
            int other_y_max = other->max_y;

            if (!(other_x_max < x_min || other_x_min > x_max))
                h_count++;
            if (!(other_y_max < y_min || other_y_min > y_max))
                v_count++;
        }

        if(h_count < min_neighbors || v_count < min_neighbors)
        {
            s->has_been_removed = 1;
            image_remove_shape(img, s);
        }
    }

    // cleanup
    clean_shapes(shapes);
}


void filter_by_density_h(Image *img,Shape **shapes, int min_neighbors)
{
    if (shapes == NULL)
        return;

    int count = 0;
    while (shapes[count] != NULL)
        count++;

    for (int i = 0; i < count; i++)
    {
        Shape *s = shapes[i];
        if (s->has_been_removed != 0)
            continue;

        int h_count = 0;

        int x_min = s->min_x;
        int x_max = s->max_x;

        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            Shape *other = shapes[j];
            if (other->has_been_removed != 0)
                continue;

            int other_x_min = other->min_x;
            int other_x_max = other->max_x;

            if (!(other_x_max < x_min || other_x_min > x_max))
                h_count++;
        }

        if(h_count < min_neighbors)
        {
            s->has_been_removed = 1;
            image_remove_shape(img, s);
        }
    }

    // cleanup
    clean_shapes(shapes);
}

void filter_by_density_v(Image *img,Shape **shapes, int min_neighbors)
{
    if (shapes == NULL)
        return;

    int count = 0;
    while (shapes[count] != NULL)
        count++;

    for (int i = 0; i < count; i++)
    {
        Shape *s = shapes[i];
        if (s->has_been_removed != 0)
            continue;

        int v_count = 0;

        int y_min = s->min_y;
        int y_max = s->max_y;

        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            Shape *other = shapes[j];
            if (other->has_been_removed != 0)
                continue;

            int other_y_min = other->min_y;
            int other_y_max = other->max_y;

            if (!(other_y_max < y_min || other_y_min > y_max))
                v_count++;
        }

        if(v_count < min_neighbors)
        {
            s->has_been_removed = 1;
            image_remove_shape(img, s);
        }
    }

    // cleanup
    clean_shapes(shapes);
}

void detect_grid_size(Shape **shapes, int *rows, int *cols)
{
    *rows = 0;
    *cols = 0;

    if (shapes == NULL)
        return;

    int count = 0;
    while (shapes[count] != NULL)
        count++;
    if (count == 0)
        return;

    // create arrays for sorting
    Shape **shapes_by_y = malloc(count * sizeof(Shape *));
    if (!shapes_by_y) {
        printf("Cannot allocate memory for shape sorting\n");
        exit(EXIT_FAILURE);
    }

    // copy shapes to new arrays
    for (int i = 0; i < count; i++) {
        shapes_by_y[i] = shapes[i];
    }

    // sort by y
    qsort(shapes_by_y, count, sizeof(Shape *), compare_shape_y);

    double avg_height = 0.0;
    for (int i = 0; i < count; i++) {
        avg_height += shape_height(shapes_by_y[i]);
    }
    avg_height /= count;

    double row_threshold = avg_height * 0.8;



    int *col_per_row = malloc(count * sizeof(int));
    if (!col_per_row) {
        printf("Cannot allocate memory for column counting\n");
        exit(EXIT_FAILURE);
    }

    int rows_count = 0;
    int curr_col_count = 0;
    double last_y = shapes[0]->min_y;
    int idx = 0;

    for (int i = 0; i < count; i++) {

        if(fabs(shapes[i]->min_y - last_y) > row_threshold) {
            // new row
            if (curr_col_count > 0)
            {
                qsort(&shapes_by_y[idx], curr_col_count, sizeof(Shape *), compare_shape_x);
                col_per_row[rows_count++] = curr_col_count;
            }
            idx = i;
            curr_col_count = 0;
        }
        curr_col_count++;
        last_y = shapes[i]->min_y;
    }

    // last row
    if (curr_col_count > 0)
    {
        qsort(&shapes_by_y[idx], curr_col_count, sizeof(Shape *), compare_shape_x);
        col_per_row[rows_count++] = curr_col_count;
    }

    if (rows_count == 0)
    {
        free(col_per_row);
        free(shapes_by_y);
        return;
    }

    qsort(col_per_row, rows_count, sizeof(int), comp);
    int median_cols = col_per_row[rows_count / 2];

    *rows = rows_count;
    *cols = median_cols;

    free(col_per_row);
    free(shapes_by_y);
}

Image ***get_grid_cells(Image *img, Shape **shapes, int rows, int cols)
{
    Image ***grid = malloc(rows * sizeof(Image **));
    if (!grid)
    {
        printf("Cannot allocate memory for grid cells\n");
        exit(EXIT_FAILURE);
    }
    for (int r = 0; r < rows; r++)
    {
        grid[r] = malloc(cols * sizeof(Image *));
        if (!grid[r])
        {
            printf("Cannot allocate memory for grid cells\n");
            exit(EXIT_FAILURE);
        }
    }

    double cell_w = (double)img->width / cols;
    double cell_h = (double)img->height / rows;

    Shape ***assigned = malloc(rows * sizeof(Shape **));
    for (int r = 0; r < rows; r++)
    {
        assigned[r] = calloc(cols, sizeof(Shape *));
    }

    for (int i = 0; shapes[i] != NULL; i++)
    {
        Shape *s = shapes[i];
        if (s->has_been_removed != 0)
            continue;

        int center_x = (s->min_x + s->max_x) / 2;
        int center_y = (s->min_y + s->max_y) / 2;

        int col = (int)(center_x / cell_w);
        int row = (int)(center_y / cell_h);

        if(col < 0) col = 0;
        if(col >= cols) col = cols -1;
        if(row < 0) row = 0;
        if(row >= rows) row = rows -1;

        double expected_cx = (col + 0.5) * cell_w;
        double expected_cy = (row + 0.5) * cell_h;

        double dist = sqrt(pow(center_x - expected_cx, 2) + pow(center_y - expected_cy, 2));

        if (assigned[row][col] != NULL)
        {
            Shape *existing = assigned[row][col];
            int existing_cx = (existing->min_x + existing->max_x) / 2;
            int existing_cy = (existing->min_y + existing->max_y) / 2;
            double existing_dist = sqrt(pow(existing_cx - expected_cx, 2) + pow(existing_cy - expected_cy, 2));

            if (dist < existing_dist)
            {
                existing->has_been_removed = 1;
                assigned[row][col] = s;
            }
            else
            {
                s->has_been_removed = 1;
            }
        }
        else
        {
            assigned[row][col] = s;
        }
    }
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if(assigned[r][c] == NULL)
            {
                grid[r][c] = NULL;
                continue;
            }

            Shape *s = assigned[r][c];
            int x_start = s->min_x;
            int y_start = s->min_y;
            int x_end = s->max_x;
            int y_end = s->max_y;

            grid[r][c] = extract_sub_image(img, x_start, y_start, x_end , y_end);
        }
    }

    for (int r = 0; r < rows; r++)
        free(assigned[r]);
    free(assigned);

    return grid;
}
Shape ***get_all_world(Shape **shapes)
{
    if (shapes == NULL)
        return NULL;

    int total_shapes = 0;
    while (shapes[total_shapes] != NULL)
        total_shapes++;

    if (total_shapes == 0)
        return NULL;

    // marker
    for (int i = 0; i < total_shapes; i++)
    {
        shapes[i]->has_been_removed = 0;
    }

    // Sort shapes by y then by x
    Shape **sorted_shapes = malloc(total_shapes * sizeof(Shape *));
    for (int i = 0; i < total_shapes; i++)
    {
        sorted_shapes[i] = shapes[i];
    }
    qsort(sorted_shapes, total_shapes, sizeof(Shape *), compare_shape_y);

    int words_capacity = 4;
    int words_count = 0;
    Shape ***words = malloc(words_capacity * sizeof(Shape **));
    if (!words)
    {
        printf("Cannot allocate memory for words\n");
        exit(EXIT_FAILURE);
    }

    // Process each shape
    for (int i = 0; i < total_shapes; i++)
    {
        if (sorted_shapes[i]->has_been_removed != 0)
            continue;

        //new word
        int word_capacity = 4;
        int word_count = 0;
        Shape **current_word = malloc(word_capacity * sizeof(Shape *));
        if (!current_word)
        {
            printf("Cannot allocate memory for current word\n");
            exit(EXIT_FAILURE);
        }

        // first shape
        current_word[word_count++] = sorted_shapes[i];
        sorted_shapes[i]->has_been_removed = 1;

        // Fix the horizontal line
        int reference_y = (sorted_shapes[i]->min_y + sorted_shapes[i]->max_y) / 2;
        int reference_height = shape_height(sorted_shapes[i]);

        // Tolerance for vertical alignment
        int vertical_tolerance = reference_height / 2;

        int *spacings = malloc(total_shapes * sizeof(int));
        int spacing_count = 0;

        // Find all shapes in the same word
        int found = 1;
        while (found)
        {
            found = 0;

            Shape *current_shape = current_word[word_count - 1];
            int current_max_x = current_shape->max_x;

            Shape *closest = NULL;
            int closest_idx = -1;
            int min_min_x = INT_MAX;

            for (int j = 0; j < total_shapes; j++)
            {
                if (sorted_shapes[j]->has_been_removed != 0)
                    continue;

                const int shape_y = (sorted_shapes[j]->min_y + sorted_shapes[j]->max_y) / 2;
                const int shape_min_x = sorted_shapes[j]->min_x;

                if (abs(shape_y - reference_y) > vertical_tolerance)
                    continue;

                if (shape_min_x < current_word[0]->min_x)
                    continue;

                if (shape_min_x < min_min_x)
                {
                    min_min_x = shape_min_x;
                    closest = sorted_shapes[j];
                    closest_idx = j;
                }
            }

            if (closest != NULL)
            {
                const int actual_spacing = closest->min_x - current_max_x;

                int accept = 0;

                if (spacing_count == 0)
                {
                    int avg_width = (shape_width(current_shape) + shape_width(closest)) / 2;

                    if (actual_spacing < 0 || actual_spacing <= avg_width * 2.5)
                        accept = 1;
                }
                else
                {
                    if (actual_spacing < 0)
                    {
                        accept = 1;
                    }
                    else
                    {
                        double sum_spacing = 0;
                        for (int k = 0; k < spacing_count; k++)
                        {
                            sum_spacing += spacings[k];
                        }
                        double avg_spacing = sum_spacing / spacing_count;

                        double threshold = avg_spacing * 4;

                        if (threshold < 40)
                            threshold = 40;

                        if (actual_spacing <= threshold)
                            accept = 1;
                    }
                }

                if (accept)
                {
                    if (word_count >= word_capacity)
                    {
                        word_capacity *= 2;
                        current_word = realloc(current_word, word_capacity * sizeof(Shape *));
                        if (!current_word)
                        {
                            printf("Cannot realloc memory for word\n");
                            exit(EXIT_FAILURE);
                        }
                    }

                    current_word[word_count++] = closest;
                    sorted_shapes[closest_idx]->has_been_removed = 1;

                    if (actual_spacing > 0)
                        spacings[spacing_count++] = actual_spacing;

                    found = 1;
                }
            }
        }

        free(spacings);

        // Sort shapes in word by x position
        qsort(current_word, word_count, sizeof(Shape *), compare_shape_x);

        // Null-terminate word array
        current_word = realloc(current_word, (word_count + 1) * sizeof(Shape *));
        current_word[word_count] = NULL;

        // Add word to words array
        if (words_count >= words_capacity)
        {
            words_capacity *= 2;
            words = realloc(words, words_capacity * sizeof(Shape **));
            if (!words)
            {
                printf("Cannot realloc memory for words\n");
                exit(EXIT_FAILURE);
            }
        }
        words[words_count++] = current_word;
    }

    // Null-terminate  words array
    words = realloc(words, (words_count + 1) * sizeof(Shape **));
    words[words_count] = NULL;

    free(sorted_shapes);

    return words;
}
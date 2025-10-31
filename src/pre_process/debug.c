#include "grayscale.h"
#include "../core/image.h"
#include "rotation.h"
#include "scale.h"
#include <time.h>
#include "grid/grid_detection.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    char *files[] = {
        "../../resources/pre_process/input/level_1_image_1.png",
        "../../resources/pre_process/input/level_1_image_2.png",
        "../../resources/pre_process/input/level_2_image_1.png",
        "../../resources/pre_process/input/level_2_image_2.png",
        "../../resources/pre_process/input/level_3_image_1.png",
        "../../resources/pre_process/input/level_3_image_2.png",
        NULL
    };

    // Load all images
    Image *imgs[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    for (int i = 0; files[i] != NULL; i++) {
        imgs[i] = load_image(files[i]);

        /* Manual Rotation for specific images */
        if (i == 2)
        {
            Image *tmp = manual_rotate_image(imgs[2], -24.5);
            free_image(imgs[2]);
            imgs[2] = tmp;
        }
        if (i == 3)
        {
            Image *tmp = manual_rotate_image(imgs[3], 4.5);
            free_image(imgs[3]);
            imgs[3] = tmp;
        }
        /* */

        /* grayscal and Binarization */
        grayscale_image(imgs[i]);

        Image *tmp = sauvola(imgs[i], 12, 128, 0.07);
        free_image(imgs[i]);
        imgs[i] = tmp;
        /* */

        /* get components connected */
        Shape **shapes = get_all_shape(imgs[i]);

        /* Filter shapes */
        remove_small_shape(imgs[i], shapes, 8);
        remove_outliers_shape(imgs[i], shapes, 25,75,2.5,3);
        remove_aspect_ration(imgs[i], shapes, 0.1, 5);

        // copy of original image
        Image *orig = copy_image(imgs[i]);

        /* Transform to circle image for better hough transform */
        Image *circle = circle_image(imgs[i], shapes, 0.25);
        free_image(imgs[i]);
        imgs[i] = circle;
        /* */

        /* Perform Hough Transform and detect lines in circle image */
        int theta_range, rho_max;
        int **hs = hough_space(imgs[i], &theta_range, &rho_max);
        // filter the hough space the only peak important lines
        hough_space_filter(hs, theta_range, rho_max, 0.495);

        // remove lines that are not 90 or 180 or 0 degrees
        // also remove lines that are too close to each other
        filter_line(hs, theta_range, rho_max, 15 ,20);

        // sort all lines to have horizontal and vertical lines
        int **h_lines = horizontal_lines(hs, theta_range, rho_max, 5);
        int **v_lines = vertical_lines(hs, theta_range, rho_max, 5);

        // filter horizontal and vertical lines independently
        // this will only keep the lines that form the biggest set with regular gaps
        filter_gaps(h_lines, theta_range, rho_max);
        filter_gaps(v_lines, theta_range, rho_max);

        // draw lines on image for debug
        draw_lines(imgs[i], h_lines, theta_range, rho_max, 255, 0, 0);
        draw_lines(imgs[i], v_lines, theta_range, rho_max, 0, 0, 255);

        // get bounding box of the grid
        int x_start, y_start, x_end, y_end;
        get_bounding_box(v_lines, h_lines, theta_range, rho_max, &x_start, &x_end, &y_start, &y_end);
//        set_pixel_color(imgs[i], x_start, y_start, 0, 255, 0);
//        set_pixel_color(imgs[i], x_end, y_end, 0, 255, 0);
//        set_pixel_color(imgs[i], x_start, y_end, 0, 255, 0);
//        set_pixel_color(imgs[i], x_end, y_start, 0, 255, 0);

        // free memory
        free_hough(h_lines, theta_range);
        free_hough(v_lines, theta_range);
        free_hough(hs, theta_range);

        clean_shapes(shapes);

        //restore original image
        free_image(imgs[i]);
        imgs[i] = orig;

        /* Adjust bounding box with mean shape size to compensate the circle image transformation */
        double mean_shape_width = 0.0, mean_shape_height = 0.0;
        int shape_count = 0;
        for (int j = 0; shapes[j] != NULL; j++)
        {
            shape_count++;
            mean_shape_width += shape_width(shapes[j]);
            mean_shape_height += shape_height(shapes[j]);
        }
        mean_shape_width /= shape_count;
        mean_shape_height /= shape_count;


        int offset_x = ceil(mean_shape_width *1.6);
        int offset_y = ceil(mean_shape_height *1.2);
        x_start = x_start - offset_x < 0 ? 0 : x_start - offset_x;
        y_start = y_start - offset_y < 0 ? 0 : y_start - offset_y;
        x_end = x_end + offset_x >= imgs[i]->width ? imgs[i]->width - 1 : x_end + offset_x;
        y_end = y_end + offset_y >= imgs[i]->height ? imgs[i]->height - 1 : y_end + offset_y;

        // if x_start is in first 5% of image width, set to 0 ans same for x_end
        if (x_start < imgs[i]->width * 0.1 && x_end > imgs[i]->width * 0.90)
        {
            x_start = offset_x;
            x_end = imgs[i]->width - offset_x;
        }
        /* */


        /* Draw bounding box of grid*/
        // draw line with a offset of 1 pixel to be more visible
        draw_line(imgs[i], x_start - 1 < 0 ? 0 : x_start - 1, y_start - 1 < 0 ? 0 : y_start - 1, x_end + 1 >= imgs[i]->width ? imgs[i]->width - 1 : x_end + 1, y_start - 1 < 0 ? 0 : y_start - 1, 0, 255, 0);
        draw_line(imgs[i], x_start - 1 < 0 ? 0 : x_start - 1, y_end + 1 >= imgs[i]->height ? imgs[i]->height - 1 : y_end + 1, x_end + 1 >= imgs[i]->width ? imgs[i]->width - 1 : x_end + 1, y_end + 1 >= imgs[i]->height ? imgs[i]->height - 1 : y_end + 1, 0, 255, 0);
        draw_line(imgs[i], x_start - 1 < 0 ? 0 : x_start - 1, y_start - 1 < 0 ? 0 : y_start - 1, x_start - 1 < 0 ? 0 : x_start - 1, y_end + 1 >= imgs[i]->height ? imgs[i]->height - 1 : y_end + 1, 0, 255, 0);
        draw_line(imgs[i], x_end + 1 >= imgs[i]->width ? imgs[i]->width - 1 : x_end + 1, y_start - 1 < 0 ? 0 : y_start - 1, x_end + 1 >= imgs[i]->width ? imgs[i]->width - 1 : x_end + 1, y_end + 1 >= imgs[i]->height ? imgs[i]->height - 1 : y_end + 1, 0, 255, 0);
        /* */

        /* Extract sub-image of the grid */
        Image *sub_img = extract_sub_image(imgs[i], x_start, y_start, x_end, y_end);


        /* Manual scaling of sub-image to have better processing later */
        if (sub_img->height > 500)
        {
            int height_objective = (int)(sub_img->height * 0.8);
            float scale_x = (float)height_objective / (float)(sub_img->height);
            Image *scale = manual_image_scaling(sub_img, scale_x, scale_x);
            free_image(sub_img);
            sub_img = scale;
        }
        else
        {
            int height_objective = (int)(sub_img->height * 1.3);
            float scale_x = (float)height_objective / (float)(sub_img->height);
            Image *scale = manual_image_scaling(sub_img, scale_x, scale_x);
            free_image(sub_img);
            sub_img = scale;
        }


        /* */

        Shape **sub_shapes = get_all_shape(sub_img);

        remove_small_shape(sub_img,sub_shapes , 15);
        remove_outliers_shape(sub_img,sub_shapes , 25,75,2.5,3);
        remove_aspect_ration(sub_img,sub_shapes , 0.1, 2);

        clean_shapes(sub_shapes);

        merge_shapes(sub_shapes, 3);

        filter_by_density(sub_img,sub_shapes, 5);
        clean_shapes(sub_shapes);

        int rows, cols;
        detect_grid_size(sub_shapes, &rows, &cols);
        //printf("Detected grid size: %d rows x %d cols\n", rows, cols);

        int min = rows < cols ? rows : cols;
        filter_by_density(sub_img, sub_shapes, (int)(min * 0.9));
        clean_shapes(sub_shapes);

        detect_grid_size(sub_shapes, &rows, &cols);
        //printf("After filtering, grid size: %d rows x %d cols\n", rows, cols);


        Image *** cell_images = get_grid_cells(sub_img,sub_shapes, rows, cols);
        //export each cell image with name cell_x_y.bmp
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (cell_images[r][c] == NULL)
                    continue;
                char cell_filename[256];
                snprintf(cell_filename, sizeof(cell_filename), "../../resources/pre_process/output/grid/cells/image%d/cell_%d_%d.bmp", i + 1, c + 1, r + 1);
                SDL_Surface *cell_surf = image_to_sdl_surface(cell_images[r][c]);
                export_image(cell_surf, cell_filename);
                SDL_FreeSurface(cell_surf);
                free_image(cell_images[r][c]);
            }
            free(cell_images[r]);
        }
        free(cell_images);

        for (int k = 0; sub_shapes[k] != NULL; ++k)
        {
            // draw bounding box of each shape
            int x_min = sub_shapes[k]->min_x;
            int y_min = sub_shapes[k]->min_y;
            int x_max = sub_shapes[k]->max_x;
            int y_max = sub_shapes[k]->max_y;
            draw_line(sub_img, x_min, y_min, x_max, y_min, 0, 255, 0);
            draw_line(sub_img, x_min, y_max, x_max, y_max, 0, 255, 0);
            draw_line(sub_img, x_min, y_min, x_min, y_max, 0, 255, 0);
            draw_line(sub_img, x_max, y_min, x_max, y_max, 0, 255, 0);
        }

        for (int j = 0; sub_shapes[j] != NULL; j++)
        {
            free_shape(sub_shapes[j]);
        }
        free(sub_shapes);

        SDL_Surface *sub_surf = image_to_sdl_surface(sub_img);
        char sub_filename[256];
        snprintf(sub_filename, sizeof(sub_filename), "../../resources/pre_process/output/grid/grid_image_%d.bmp", i + 1);
        export_image(sub_surf, sub_filename);
        SDL_FreeSurface(sub_surf);



        free_image(sub_img);


        for (int j = 0; shapes[j] != NULL; j++)
        {
            free_shape(shapes[j]);
        }
        free(shapes);


        // Export
        char out_filename[256];
        snprintf(out_filename, sizeof(out_filename), "processed_image_%d.bmp", i + 1);
        SDL_Surface *surf = image_to_sdl_surface(imgs[i]);
        if (surf) {
            export_image(surf, out_filename);
            SDL_FreeSurface(surf);
        } else {
            printf("Failed to convert image to SDL_Surface: %s\n", files[i]);
        }
    }



    for (int i = 0; imgs[i] != NULL; i++)
        free_image(imgs[i]);

    cleanup_hidden_renderer_scale();
    cleanup_hidden_renderer();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

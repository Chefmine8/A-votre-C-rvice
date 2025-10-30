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

    Image *imgs[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    for (int i = 0; files[i] != NULL; i++) {
        imgs[i] = load_image(files[i]);

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

        grayscale_image(imgs[i]);

        Image *tmp = sauvola(imgs[i], 12, 128, 0.07);
        free_image(imgs[i]);
        imgs[i] = tmp;


        Shape **shapes = get_all_shape(imgs[i]);

        remove_small_shape(imgs[i], shapes, 8);
        remove_outliers_shape(imgs[i], shapes, 25,75,2.5,3);
        remove_aspect_ration(imgs[i], shapes, 0.1, 5);

        Image *circle = circle_image(imgs[i], shapes, 0.25);
        free_image(imgs[i]);
        imgs[i] = circle;

        int theta_range, rho_max;
        int **hs = hough_space(imgs[i], &theta_range, &rho_max);
        hough_space_filter(hs, theta_range, rho_max, 0.495);
        filter_line(hs, theta_range, rho_max, 15 ,20);

        draw_lines(imgs[i], hs, theta_range, rho_max);

        free_hough(hs, theta_range, rho_max);

        clean_shapes(shapes);

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


    // for (int i = 0; files[i] != NULL; i++) {
    //     Image *scale = resize_image(imgs[i], 600, 600, true);
    //     free_image(imgs[i]);
    //     imgs[i] = scale;
    // }
    // Image *img =
    //     load_image("../../resources/pre_process/input/level_1_image_2.png");
    //
    // Image *scale = resize_image(img, 800, 800, true);
    // free_image(img);
    //
    // SDL_Surface *surf1 = image_to_sdl_surface(scale);
    // export_image(surf1, "scale_output.bmp");
    // SDL_FreeSurface(surf1);  // ✅
    //
    // Image *rot = manual_rotate_image(scale, 20);
    // free_image(scale);
    //
    // grayscale_image(rot);
    //
    // Image *bin = sauvola(rot, 10, 128, 0.07);
    // free_image(rot);
    //
    // SDL_Surface *surf2 = image_to_sdl_surface(bin);
    // export_image(surf2, "output.bmp");
    // SDL_FreeSurface(surf2);  // ✅
    //
    // // long double *nn_input = malloc(28 * 28 * sizeof(long double));
    // // get_nn_input(bin, nn_input);
    // // for (int i = 0; i < 28 * 28; i++)
    // // {
    // //     printf("%d ", (int)nn_input[i]);
    // // }
    // // printf("\n");
    // free_image(bin);
    //free(nn_input);  // ✅


    for (int i = 0; imgs[i] != NULL; i++)
        free_image(imgs[i]);

    cleanup_hidden_renderer();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

#include "grayscale.h"
#include "../core/image.h"
#include "rotation.h"
#include "scale.h"
#include <time.h>

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

    Image *imgs[] = { NULL, NULL, NULL, NULL, NULL, NULL };
    for (int i = 0; files[i] != NULL; i++) {
        imgs[i] = load_image(files[i]);
        }

    /*
    Image *rot = manual_rotate_image(imgs[2], -25);
    free_image(imgs[2]);
    imgs[2] = rot;

    Image *rot2 = manual_rotate_image(imgs[3], 5);
    free_image(imgs[3]);
    imgs[3] = rot2;

    // for (int i = 0; files[i] != NULL; i++) {
    //     Image *scale = resize_image(imgs[i], 600, 600, true);
    //     free_image(imgs[i]);
    //     imgs[i] = scale;
    // }


    for (int i = 0; files[i] != NULL; i++) {
        grayscale_image(imgs[i]);
    }
    for (int i = 0; files[i] != NULL; i++) {
        Image *bin = sauvola(imgs[i], 12, 128, 0.07);
        free_image(imgs[i]);
        imgs[i] = bin;
    }


    // export
    for (int i = 0; files[i] != NULL; i++) {
        char output_file[256];
        snprintf(output_file, sizeof(output_file), "../../resources/pre_process/output/output_image_%d.bmp", i + 1);
        SDL_Surface *surf = image_to_sdl_surface(imgs[i]);
        export_image(surf, output_file);
        SDL_FreeSurface(surf);  // ✅
        free_image(imgs[i]);
    }

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
    */

    Image * test = imgs[0];
    grayscale_image(test);


    Image *bin = sauvola(test, 12, 128, 0.07);
    test = bin;


    for (int x = 0; x < test->width; x++)
    {

        for (int y = 0; y < test->height; y++)
        {
            Pixel *p = get_pixel(test, x, y);
            printf("%i\n", p->r);
        }
    }

    Shape **shapes = get_all_shape(test);

    // Seed random generator
    srand((unsigned int)time(NULL));

    // Assign a random color to each shape
    for (int i = 0; shapes[i] != NULL; i++)
    {
        uint8_t r = rand() % 256;
        uint8_t g = rand() % 256;
        uint8_t b = rand() % 256;

        //printf("test %i", shapes[i]->count);
        image_change_shape_color(test, shapes[i], r, g, b);
        //image_remove_shape(test, shapes[i]);
    }

    SDL_Surface *surf = image_to_sdl_surface(test);
    export_image(surf, "test_shapes_colored.bmp");
    SDL_FreeSurface(surf);

    for (int i = 0; shapes[i] != NULL; i++)
    {
        free_shape(shapes[i]);
    }
    free(shapes);

    for (int i = 0; imgs[i] != NULL; i++)
        free_image(imgs[i]);
    free_image(test);


    IMG_Quit();
    SDL_Quit();
    return 0;
}

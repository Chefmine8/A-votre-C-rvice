#include "grayscale.h"
#include "../core/image.h"
#include "rotation.h"
#include "scale.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    Image *img =
        load_image("../../resources/pre_process/input/level_1_image_2.png");

    Image *scale = resize_image(img, 800, 800, true);
    free_image(img);

    SDL_Surface *surf1 = image_to_sdl_surface(scale);
    export_image(surf1, "scale_output.bmp");
    SDL_FreeSurface(surf1);  // ✅

    Image *rot = manual_rotate_image(scale, 20);
    free_image(scale);

    grayscale_image(rot);

    Image *bin = sauvola(rot, 10, 128, 0.07);
    free_image(rot);

    SDL_Surface *surf2 = image_to_sdl_surface(bin);
    export_image(surf2, "output.bmp");
    SDL_FreeSurface(surf2);  // ✅

    // long double *nn_input = malloc(28 * 28 * sizeof(long double));
    // get_nn_input(bin, nn_input);
    // for (int i = 0; i < 28 * 28; i++)
    // {
    //     printf("%d ", (int)nn_input[i]);
    // }
    // printf("\n");
    free_image(bin);
    //free(nn_input);  // ✅

    IMG_Quit();
    SDL_Quit();
    return 0;
}

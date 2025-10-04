#include "grayscale.h"
#include "../core/image.h"
#include "rotation.h"
#include "scale.h"

int main()
{
    Image *img =
        load_image("../../resources/pre_process/input/level_1_image_2.png");

    Image *scale = resize_image(img, 50, 50, true);

    export_image(image_to_sdl_surface(scale), "scale_output.bmp");

    Image *rot = manual_rotate_image(scale, 20);

    grayscale_image(rot);

    Image *bin = sauvola(rot, 10, 128, 0.07);

    export_image(image_to_sdl_surface(bin), "output.bmp");

    long double *nn_input = get_nn_input(bin);
    for (int i = 0; i < 28 * 28; i++)
    {
        printf("%d ", (int)nn_input[i]);
    }
    printf("\n");
}

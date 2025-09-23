#include "grayscale.h"
#include "image.h"
#include "rotation.h"

int main()
{
    Image *img =
        load_image("../../resources/pre_process/input/level_2_image_2.png");

    Image *rot = manual_rotate_image(img, 20);

    grayscale_image(rot);

    Image *bin = sauvola(rot, 10, 128, 0.07);

    export_image(get_sdl_surface(bin), "output.bmp");
}

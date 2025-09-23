#include "grayscale.h"
#include "image.h"

int main()
{
    Image *img =
        loadImage("../../resources/pre_process/input/level_2_image_2.png");

    Image *rot = rotateImage(img, 20);

    grayscaleImage(rot);

    Image *bin = sauvola(rot, 10, 128, 0.07);

    exportImage(getSDL_Surface(bin), "output.bmp");
}

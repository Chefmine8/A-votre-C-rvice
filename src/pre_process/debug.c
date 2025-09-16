#include "grayscale.h"
#include "image.h"

int main()
{
    Image *img =
        loadImage("../../resources/pre_process/input/level_3_image_2.png");

    grayscaleImage(img);

    Image *copy = sauvola(img, 10, 128, 0.7);

    exportImage(copy, "output.bmp");
}

#include "grayscale.h"
#include "image.h"

int main()
{
    Image *img =
        loadImage("../../resources/pre_process/input/level_3_image_2.png");

    grayscaleImage(img);

    Image *copy = sauvola(img, 5, 128, 0.5);

    exportImage(copy, "output.bmp");
}

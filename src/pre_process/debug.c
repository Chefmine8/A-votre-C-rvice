#include "grayscale.h"
#include "image.h"

int main()
{
    Image *img =
        loadImage("../../resources/pre_process/input/level_2_image_2.png");

    grayscaleImage(img);

    Image *copy = sauvola(img, 10, 128, 0.07);

    exportImage(copy, "output.bmp");
}

#include "grayscale.h"


int main()
{
    Image *img = loadImage("../../resources/pre_process/input/level_3_image_2.png");

    grayscaleImage(img);

    exportImage(img, "output.bmp");
}

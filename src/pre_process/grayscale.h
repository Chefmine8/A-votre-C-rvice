#pragma once

#include "image.h"
#include <stdint.h>
void grayscaleImage(Image *img);
Image *sauvola(Image *img, int n, int R, float k);

#pragma once

#include "image.h"
#include <stdint.h>
void grayscaleImage(const Image *img);
Image *sauvola(const Image *img, int n, int R, float k);

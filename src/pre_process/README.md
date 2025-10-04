
# Pre-processing Module (WIP / Not Finished)

Status: **IN PROGRESS**

## Overview

This part implements basic image pre-processing utilities on top of SDL2:
- Minimal `Image` abstraction wrapping a 2D `Pixel` matrix (`uint8_t r,g,b`)
- Conversion helpers between `SDL_Surface` and `Image`
- Grayscale conversion (luminance formula: 0.2125 R + 0.7154 G + 0.0721 B)
- Sauvola adaptive thresholding (local binarization)
- Manual image rotation using an off-screen SDL renderer / textures
- Simple export to BMP


## Dependencies

- SDL2
- SDL2_image
- math library (`-lm`)

On Debian/Ubuntu:

```
sudo apt install libsdl2-dev libsdl2-image-dev
```

## Build

From `src/pre_process`:
```
make test
```
This finds all `*.c` recursively and builds the test executable `test` based on `main` in `debug.c`.

Clean:
```
make clean
```

## Usage Examples

Minimal load → scale → export → rotate → grayscale → sauvola → export

Always free resources after use.
```c
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
```

## Sauvola Parameters

| Param | Meaning | Typical |
|-------|---------|---------|
| n     | Window size (odd) | 15, 25 |
| R     | Std-dev range     | 128 |
| k     | Contrast factor   | 0.2–0.5 |

Threshold formula: `T = m * (1 + k * ((s / R) - 1))`

## Neural Network Input
Resize to 28x28 and normalize pixel values to [0, 1]. The function `get_nn_input` take an pre-allocated array of `long double` of size 784 (28*28).

### Example of usage 
```c
#include "../core/image.h"
#include "scale.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    Image *img =
        load_image("../../resources/pre_process/input/level_1_image_2.png");

    Image *scale = resize_image(img, 28, 28, true);
    free_image(img);

    long double *nn_input = malloc(28 * 28 * sizeof(long double));
    get_nn_input(scale, nn_input);
    for (int i = 0; i < 28 * 28; i++)
    {
        printf("%Lf ", nn_input[i]);
    }
    printf("\n");
    free_image(scale);
    free(nn_input);  // ✅

    IMG_Quit();
    SDL_Quit();
    return 0;
}
```
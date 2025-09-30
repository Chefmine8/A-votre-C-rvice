
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

Minimal load → grayscale → Sauvola → export:
```c
#include "core/image.h"
#include "pre_process/grayscale.h"
#include "pre_process/rotation.h"

int main(void) {

    Image *img = load_image("input.png");
    grayscale_image(img);

    Image *bin = sauvola(img, 15, 128, 0.5f); // block size 15
    SDL_Surface *surf = image_to_sdl_surface(bin);
    export_image(surf, "binarized.bmp");

    Image *rot = manual_rotate_image(bin, 15.0);
    SDL_Surface *rotSurf = image_to_sdl_surface(rot);
    export_image(rotSurf, "rotated.bmp");

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


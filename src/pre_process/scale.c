#include "scale.h"

Image *manual_image_scaling(const Image *src, const float scale_x, const float scale_y)
{
    if (!src)
        return NULL;

    const int new_w = (int)(src->width * scale_x);
    const int new_h = (int)(src->height * scale_y);

    // SDL Init
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // window and renderer (hidden)
    SDL_Window *win =
        SDL_CreateWindow("Hidden", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_HIDDEN);
    SDL_Renderer *renderer =
        SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *tex =
        SDL_CreateTextureFromSurface(renderer, image_to_sdl_surface(src));

    // texture target
    SDL_Texture *target =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);


    const SDL_Rect dstRect = {0, 0, new_w, new_h };
    // rotate texture using SDL_RenderCopyEx
    SDL_RenderCopy(renderer, tex, NULL, &dstRect);

    // read pixels from renderer to create new surface
    SDL_Surface *resultSurf = SDL_CreateRGBSurfaceWithFormat(
        0, new_w, new_h, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888,
                         resultSurf->pixels, resultSurf->pitch);

    // create new Image based on surface
    Image *result = create_image(new_w, new_h);

    sdl_surface_to_image(resultSurf, result);

    // free
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(target);
    SDL_FreeSurface(resultSurf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return result;

}

Image *resize_image_square(const Image *src)
{
    const int max = (src->width > src->height) ? src->width : src->height;
    Image *output = create_image(max, max);
    const int offset_x = (max - src->width) / 2;
    const int offset_y = (max - src->height) / 2;
    for (int i = 0; i < src->width; i++) {
        for (int j = 0; j < src->height; j++) {
            Pixel *p = get_pixel(src, i, j);
            set_pixel(output, i + offset_x, j + offset_y, p);
        }
    }
    return output;

}

Image *resize_image(const Image *src, const int width, const int height, const bool keep_aspect_ratio)
{
    if (keep_aspect_ratio) {
        const Image *square = resize_image_square(src);
        const float scale_x = (float)width / (float)square->width;
        const float scale_y = (float)height / (float)square->height;
        return manual_image_scaling(square, scale_x, scale_y);
    }

    const float scale_x = (float)width / (float)src->width;
    const float scale_y = (float)height / (float)src->height;
    return manual_image_scaling(src, scale_x, scale_y);
}

long double *get_nn_input(const Image *img)
{
    long double *input = malloc(sizeof(long double) * 28 * 28);
    if (!input) {
        printf("Memory allocation failed for NN input\n");
        exit(EXIT_FAILURE);
    }
    const Image *resized = resize_image(img, 28, 28, true);
    int index = 0;
    for (int j = 0; j < resized->height; j++) {
        for (int i = 0; i < resized->width; i++) {
            const Pixel *p = get_pixel(resized, i, j);
            // normalize to [0, 1]
            input[index++] = (long double)(p->r) / 255.0;
        }
    }
    return input;
}
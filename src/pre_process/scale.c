#include "scale.h"

static SDL_Window *hidden_window = NULL;
static SDL_Renderer *hidden_renderer = NULL;

static void ensure_hidden_renderer(void) {
    if (!hidden_window) {
        hidden_window = SDL_CreateWindow("Hidden", SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_HIDDEN);
        hidden_renderer = SDL_CreateRenderer(hidden_window, -1, SDL_RENDERER_ACCELERATED);
    }
}

Image *manual_image_scaling(const Image *src, const float scale_x, const float scale_y)
{
    if (!src)
        return NULL;

    const int new_w = (int)(src->width * scale_x);
    const int new_h = (int)(src->height * scale_y);

    ensure_hidden_renderer();

    SDL_Surface *temp_surf = image_to_sdl_surface(src);
    SDL_Texture *tex =
        SDL_CreateTextureFromSurface(hidden_renderer, temp_surf);
    SDL_FreeSurface(temp_surf);

    // texture target
    SDL_Texture *target =
        SDL_CreateTexture(hidden_renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    SDL_SetRenderTarget(hidden_renderer, target);
    SDL_SetRenderDrawColor(hidden_renderer, 255, 255, 255, 255);
    SDL_RenderClear(hidden_renderer);


    const SDL_Rect dstRect = {0, 0, new_w, new_h };
    // rotate texture using SDL_RenderCopyEx
    SDL_RenderCopy(hidden_renderer, tex, NULL, &dstRect);

    // read pixels from renderer to create new surface
    SDL_Surface *resultSurf = SDL_CreateRGBSurfaceWithFormat(
        0, new_w, new_h, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_RenderReadPixels(hidden_renderer, NULL, SDL_PIXELFORMAT_RGBA8888,
                         resultSurf->pixels, resultSurf->pitch);

    // create new Image based on surface
    Image *result = create_image(new_w, new_h);

    sdl_surface_to_image(resultSurf, result);

    // free
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(target);
    SDL_FreeSurface(resultSurf);

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
        Image *square = resize_image_square(src);
        const float scale_x = (float)width / (float)square->width;
        const float scale_y = (float)height / (float)square->height;
        Image *res =  manual_image_scaling(square, scale_x, scale_y);
        free_image(square);
        return res;
    }

    const float scale_x = (float)width / (float)src->width;
    const float scale_y = (float)height / (float)src->height;
    return manual_image_scaling(src, scale_x, scale_y);
}

void get_nn_input(const Image *img, long double *input)
{
    if (!input) {
        printf("Error: input array pointer is NULL in get_nn_input()\n");
        exit(EXIT_FAILURE);
    }
    Image *resized = resize_image(img, 28, 28, true);
    int index = 0;
    for (int j = 0; j < resized->height; j++) {
        for (int i = 0; i < resized->width; i++) {
            const Pixel *p = get_pixel(resized, i, j);
            // Convert to grayscale using standard luminance formula and normalize to [0, 1]
            long double gray = (0.299 * p->r + 0.587 * p->g + 0.114 * p->b) / 255.0;
            input[index++] = gray;
        }
    }
    free_image(resized);
}


void cleanup_hidden_renderer_scale() {
    if (hidden_renderer) {
        SDL_DestroyRenderer(hidden_renderer);
        hidden_renderer = NULL;
    }
    if (hidden_window) {
        SDL_DestroyWindow(hidden_window);
        hidden_window = NULL;
    }
}
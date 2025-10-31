#include "rotation.h"

static SDL_Window *hidden_window = NULL;
static SDL_Renderer *hidden_renderer = NULL;

static void ensure_hidden_renderer(void)
{
    if (!hidden_window)
    {
        hidden_window =
            SDL_CreateWindow("Hidden", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_HIDDEN);
        hidden_renderer =
            SDL_CreateRenderer(hidden_window, -1, SDL_RENDERER_ACCELERATED);
    }
}

// roate Image using SDL Texture
Image *manual_rotate_image(const Image *src, const double angle)
{
    if (!src)
        return NULL;

    // window and renderer (hidden)
    ensure_hidden_renderer();

    SDL_Surface *temp_surf = image_to_sdl_surface(src);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(hidden_renderer, temp_surf);
    SDL_FreeSurface(temp_surf);

    // angle in radian
    const double rad = angle * M_PI / 180;

    // new width and height after rotation
    const int new_w =
        src->width * fabs(cos(rad)) + src->height * fabs(sin(rad));
    const int new_h =
        src->width * fabs(sin(rad)) + src->height * fabs(cos(rad));

    // texture target
    SDL_Texture *target =
        SDL_CreateTexture(hidden_renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    SDL_SetRenderTarget(hidden_renderer, target);
    SDL_SetRenderDrawColor(hidden_renderer, 255, 255, 255, 255);
    SDL_RenderClear(hidden_renderer);

    // center of rotation and destination rect
    const SDL_Point center = {src->width / 2, src->height / 2};
    const SDL_Rect dstRect = {(new_w - src->width) / 2,
                              (new_h - src->height) / 2, src->width,
                              src->height};
    // rotate texture using SDL_RenderCopyEx
    SDL_RenderCopyEx(hidden_renderer, tex, NULL, &dstRect, angle, &center,
                     SDL_FLIP_NONE);

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

void cleanup_hidden_renderer()
{
    if (hidden_renderer)
    {
        SDL_DestroyRenderer(hidden_renderer);
        hidden_renderer = NULL;
    }
    if (hidden_window)
    {
        SDL_DestroyWindow(hidden_window);
        hidden_window = NULL;
    }
}
#include "rotation.h"

// https://www.youtube.com/watch?v=XRBc_xkZREg&t=117s


// roate Image using SDL Texture
Image *manual_rotate_image(const Image *src, const double angle)
{
    if (!src)
        return NULL;

    // Init 
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

    // angle in radian
    const double rad = angle * M_PI / 180;

    // new width and height after rotation
    const int new_w = src->width * fabs(cos(rad)) + src->height * fabs(sin(rad));
    const int new_h = src->width * fabs(sin(rad)) + src->height * fabs(cos(rad));

    // texture target
    SDL_Texture *target =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // center of rotation and destination rect
    const SDL_Point center = {src->width / 2, src->height / 2};
    const SDL_Rect dstRect = {(new_w - src->width) / 2, (new_h - src->height) / 2,
                        src->width, src->height};
    // rotate texture using SDL_RenderCopyEx
    SDL_RenderCopyEx(renderer, tex, NULL, &dstRect, angle, &center,
                     SDL_FLIP_NONE);

    // read pixels from renderer to create new surface
    SDL_Surface *resultSurf = SDL_CreateRGBSurfaceWithFormat(
        0, new_w, new_h, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888,
                         resultSurf->pixels, resultSurf->pitch);

    // create new Image based on surface
    Image *result = create_image(new_w, new_h);

    for (int y = 0; y < new_h; y++)
    {
        for (int x = 0; x < new_w; x++)
        {
            Uint8 *pixels = (Uint8 *)resultSurf->pixels;
            const int bpp = resultSurf->format->BytesPerPixel;

            Uint8 *pPixel = pixels + y * resultSurf->pitch + x * bpp;

            Uint8 r, g, b;
            SDL_GetRGB(*(Uint32 *)pPixel, resultSurf->format, &r, &g, &b);
            Pixel p = {r, g, b};
            set_pixel(result, x, y, &p);
            // printf("Pixel (%d,%d) = R:%d G:%d B:%d\n", x, y, r, g, b);
        }
    }

    // free
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(target);
    SDL_FreeSurface(resultSurf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return result;
}
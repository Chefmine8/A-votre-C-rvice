#include "rotation.h"
#include "../grid/grid_detection.h"
#include "../utils/shapes.h"

static SDL_Window *hidden_window = NULL;
static SDL_Renderer *hidden_renderer = NULL;

static void ensure_hidden_renderer(void) {
    if (!hidden_window) {
        hidden_window = SDL_CreateWindow("Hidden", SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_HIDDEN);
        hidden_renderer = SDL_CreateRenderer(hidden_window, -1, SDL_RENDERER_ACCELERATED);
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
    SDL_Texture *tex =
        SDL_CreateTextureFromSurface(hidden_renderer, temp_surf);
    SDL_FreeSurface(temp_surf);

    // angle in radian
    const double rad = angle * M_PI / 180;

    // new width and height after rotation
    const int new_w = src->width * fabs(cos(rad)) + src->height * fabs(sin(rad));
    const int new_h = src->width * fabs(sin(rad)) + src->height * fabs(cos(rad));

    // texture target
    SDL_Texture *target =
        SDL_CreateTexture(hidden_renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    SDL_SetRenderTarget(hidden_renderer, target);
    SDL_SetRenderDrawColor(hidden_renderer, 255, 255, 255, 255);
    SDL_RenderClear(hidden_renderer);

    // center of rotation and destination rect
    const SDL_Point center = {src->width / 2, src->height / 2};
    const SDL_Rect dstRect = {(new_w - src->width) / 2, (new_h - src->height) / 2,
                        src->width, src->height};
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

double get_auto_rotation_angle(const Image *img)
{
    Image* copy = copy_image(img);
    Shape **shapes = get_all_shape(copy);

    remove_small_shape(copy, shapes, 8);
    remove_outliers_shape(copy, shapes, 25,75,2.5,3);
    remove_aspect_ration(copy, shapes, 0.1, 5);
    Image *circle_img = circle_image(copy, shapes, 0.25);

    // //export the circle image for debug use randon id in filename to avoid overwrite
    // SDL_Surface *circle_surf = image_to_sdl_surface(circle_img);
    // char circle_filename[256];
    // snprintf(circle_filename, sizeof(circle_filename), "../../resources/pre_process/output/circle_image_%ld.bmp", random());
    // export_image(circle_surf, circle_filename);
    // SDL_FreeSurface(circle_surf);
    free_image(copy);
    int theta_range, rho_max;
    int **hs = hough_space(circle_img, &theta_range, &rho_max);
    hough_space_filter(hs, theta_range, rho_max, 0.75);
    //get the line that are the most horizontal
    int **h_lines = horizontal_lines(hs, theta_range, rho_max, 30);

    // //for debug draw all line and export image
    // Image *debug_img = copy_image(circle_img);
    // draw_lines(debug_img, h_lines, theta_range, rho_max, 255, 0, 0);
    // SDL_Surface *debug_surf = image_to_sdl_surface(debug_img);
    // char debug_filename[256];
    // snprintf(debug_filename, sizeof(debug_filename), "../../resources/pre_process/output/horizontal_lines_%ld.bmp", random());
    // export_image(debug_surf, debug_filename);
    // SDL_FreeSurface(debug_surf);
    // free_image(debug_img);

    // get the average angle of these lines
    double total_angle = 0.0;
    int count = 0;
    for (int r = 0; r < theta_range; r++)
    {
        for (int d = 0; d < 2 * rho_max; d++)
        {
            if (h_lines[r][d] > 0)
            {
                double angle = (double)r - 90.0; // convert to -90 to 90 degrees
                total_angle += angle;
                count++;
            }
        }
    }
    double average_angle = 0.0;
    if (count > 0)
    {
            average_angle = total_angle / (double)count;
    }
    // free memory
    free_hough(h_lines, theta_range);
    free_hough(hs, theta_range);
    free_image(circle_img);
    for (int j = 0; shapes[j] != NULL; j++)
    {
            free_shape(shapes[j]);
    }
    free(shapes);
    return average_angle * 1.12;
}

void cleanup_hidden_renderer() {
    if (hidden_renderer) {
        SDL_DestroyRenderer(hidden_renderer);
        hidden_renderer = NULL;
    }
    if (hidden_window) {
        SDL_DestroyWindow(hidden_window);
        hidden_window = NULL;
    }
}
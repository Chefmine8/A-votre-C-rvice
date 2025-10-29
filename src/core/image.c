#include "image.h"

// use to alloc memory for a new image
Image *create_image(const int width, const int height)
{
    Image *img = malloc(sizeof(Image));
    if (!img)
    {
        printf("Cannot allocate memory for image\n");
        exit(EXIT_FAILURE);
    }
    img->width = width;
    img->height = height;

    img->pixels = malloc(height * sizeof(Pixel *));
    if (!img->pixels)
    {
        printf("Cannot allocate memory for image pixels\n");
        exit(EXIT_FAILURE);
    }
    for (int y = 0; y < height; y++)
    {
        img->pixels[y] = malloc(width * sizeof(Pixel));
        if (!img->pixels[y])
        {
            printf("Cannot allocate memory for image pixels row\n");
            exit(EXIT_FAILURE);
        }
        // init to white
        for (int x = 0; x < width; x++) {
            img->pixels[y][x].r = 255;
            img->pixels[y][x].g = 255;
            img->pixels[y][x].b = 255;
        }
    }

    return img;
}

void free_image(Image *img)
{
    if (img)
    {
        for (int y = 0; y < img->height; y++)
        {
            free(img->pixels[y]);
        }
        free(img->pixels);
        free(img);
    }
}

// get pixel at (x, y) return struct Pixel
Pixel *get_pixel(const Image *img, const int x, const int y)
{
    if (!(x >= 0 && x < img->width && y >= 0 && y < img->height))
    {
        printf("Cannot read pixel (x:%d, y:%d)\n", x, y);
        exit(EXIT_FAILURE);
    }
    return &img->pixels[y][x];
}

// change tu pixel at (x, y)
void set_pixel(const Image *img, const int x, const int y, const Pixel *p)
{
    if (x >= 0 && x < img->width && y >= 0 && y < img->height)
    {
        if (img->pixels != NULL)
            img->pixels[y][x] = *p;
    }
}

// change the pixel color at (x,y)
void set_pixel_color(const Image *img, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x >= 0 && x < img->width && y >= 0 && y < img->height)
    {
        if (img->pixels != NULL)
        {
            img->pixels[y][x].r = r;
            img->pixels[y][x].g = g;
            img->pixels[y][x].b = b;
        }
    }
}

// create a new image from a image struct
Image *copy_image(const Image *img)
{
    Image *new = create_image(img->width, img->height);
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            const Pixel *newP = get_pixel(img, x, y);
            set_pixel(new, x, y, newP);
        }
    }
    return new;
}


// convert SDL_Surface to custom struct Image
// if img is NULL a new image is created else the surface is copied in img inplace
Image *sdl_surface_to_image(const SDL_Surface *surf, Image *img)
{
    if (!img)
    {
        img = create_image(surf->w, surf->h);
    }
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            Uint8 *pixels = (Uint8 *)surf->pixels;
            const int bpp = surf->format->BytesPerPixel;

            Uint8 *pPixel = pixels + y * surf->pitch + x * bpp;

            Uint8 r, g, b;
            SDL_GetRGB(*(Uint32 *)pPixel, surf->format, &r, &g, &b);
            Pixel p = {r, g, b, x, y, 0, NULL};
            set_pixel(img, x, y, &p);
        }
    }
    return img;
}

// convert a custom struct Image to a SDL_Surface to export or display
SDL_Surface *image_to_sdl_surface(const Image *img)
{
    SDL_Surface *surf =
        SDL_CreateRGBSurfaceWithFormat(0,           // flags
                                       img->width,  // largeur
                                       img->height, // hauteur
                                       24, // profondeur (bits per pixel)
                                       SDL_PIXELFORMAT_RGB24 // format
        );

    if (!surf)
    {
        printf("Erreur création surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            const Pixel *p = get_pixel(img, x, y);
            Uint8 *pixel = (Uint8 *)surf->pixels + y * surf->pitch + x * 3;
            pixel[0] = p->r;
            pixel[1] = p->g;
            pixel[2] = p->b;
        }
    }

    return surf;
}

// load a image (bmp, png, jpg..) and convert to custom struc Image to process
Image *load_image(const char *file)
{
    SDL_Surface *img = IMG_Load(file);
    if (img == NULL)
    {
        printf("Failed to load the file\n");
        exit(EXIT_FAILURE);
    }
    Image *img_load = sdl_surface_to_image(img, NULL);
    SDL_FreeSurface(img);
    return img_load;
}

// export bmp file of image
void export_image(SDL_Surface *surf, const char *file)
{
    SDL_SaveBMP(surf, file);
}


#include "../../core/image.h"
#include "shapes.h"

Shape* create_shape()
{
    Shape *shape = malloc(sizeof(Shape));
    if (!shape)
    {
        printf("Cannot allocate memory for the shape\n");
        exit(EXIT_FAILURE);
    }
    shape->count = 0;
    shape->max_x = 0;
    shape->max_y = 0;
    shape->min_x = 0;
    shape->min_y = 0;
    shape->capacity = 4;

    shape->pixels = malloc(shape->capacity * sizeof(Pixel));
    if (!shape->pixels)
    {
        printf("Cannot allocate memory for the pixels of the shape\n");
        exit(EXIT_FAILURE);
    }
    return shape;
}

void shape_add_pixel(Shape *s, Pixel *p)
{
    if(!s)
    {
        printf("Cannot add pixel to NULL shape\n");
        exit(EXIT_FAILURE);
    }
    if(!p)
    {
        printf("Cannot add NULL pixel to shape\n");
        exit(EXIT_FAILURE);
    }
    if (p->isInShape == 1)
    {
        printf("pixel is already in a shape\n");
        return;
    }
    p->isInShape = 1;
    p->shape_ptr = s;

    if(s->count + 1 >= s->capacity)
    {
        s->capacity *= 2;
        s->pixels = realloc(s->pixels, s->capacity * sizeof (Pixel));
        if(!s->pixels)
        {
            printf("Realloc failed\n");
            free_shape(s);
            exit(EXIT_FAILURE);
        }
    }
    s->pixels[s->count++] = *p;
}

void image_change_shape_color(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < s->count; ++i) {
        Pixel curr = s->pixels[i];
        set_pixel_color(img, curr.x, curr.y, r, g, b);
    }
}

void image_remove_shape(Image *img, Shape *s)
{
    for (int i = 0; i < s->count; ++i) {
        Pixel curr = s->pixels[i];
        curr.isInShape = 0;
        curr.shape_ptr = NULL;
        set_pixel_color(img, curr.x, curr.y, 255, 255, 255);
    }
}

void image_add_shape(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < s->count; ++i) {
        Pixel curr = s->pixels[i];
        set_pixel_color(img, curr.x, curr.y, r, g, b);
    }
}


void free_shape(Shape *s)
{
    free(s->pixels);
    free(s);
}
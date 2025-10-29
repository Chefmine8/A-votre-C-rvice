#include "../../core/image.h"
#include "shapes.h"
#include <limits.h>


Shape* create_shape()
{
    Shape *shape = malloc(sizeof(Shape));
    if (!shape)
    {
        printf("Cannot allocate memory for the shape\n");
        exit(EXIT_FAILURE);
    }
    shape->count = 0;
    shape->max_x = INT_MIN;
    shape->max_y = INT_MIN;
    shape->min_x = INT_MAX;
    shape->min_y = INT_MAX;
    shape->capacity = 4;

    shape->pixels = malloc(shape->capacity * sizeof(Pixel*));
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


    if(s->count >= s->capacity)
    {
        s->capacity *= 2;
        s->pixels = realloc(s->pixels, s->capacity * sizeof (Pixel*));
        if(!s->pixels)
        {
            printf("Realloc failed\n");
            free_shape(s);
            exit(EXIT_FAILURE);
        }
    }
    s->pixels[s->count++] = p;
    p->isInShape = 1;
    p->shape_ptr = s;

    if (s->count == 0) {
        s->max_x = s->min_x = p->x;
        s->max_y = s->min_y = p->y;
    } else {
        if (p->x > s->max_x) s->max_x = p->x;
        if (p->y > s->max_y) s->max_y = p->y;
        if (p->x < s->min_x) s->min_x = p->x;
        if (p->y < s->min_y) s->min_y = p->y;
    }
}

void image_change_shape_color(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < s->count; ++i) {
        Pixel *curr = s->pixels[i];
        set_pixel_color(img, curr->x, curr->y, r, g, b);
    }
}

void image_remove_shape(Image *img, Shape *s)
{
    for (int i = 0; i < s->count; ++i) {
        Pixel *curr = s->pixels[i];
        curr->isInShape = 0;
        curr->shape_ptr = NULL;
        set_pixel_color(img, curr->x, curr->y, 255, 255, 255);
    }
}

void image_add_shape(Image *img, Shape *s, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < s->count; ++i) {
        Pixel *curr = s->pixels[i];
        set_pixel_color(img, curr->x, curr->y, r, g, b);
    }
}


int shape_width(Shape *s)
{
    return s->max_x - s->min_x + 1;
}

int shape_height(Shape *s)
{
    return s->max_y - s->min_y + 1;
}

int shape_area(Shape *s)
{
    return shape_width(s) * shape_height(s);
}

double shape_aspect_ratio(Shape *s)
{
    int h = shape_height(s);
    double w = (double)shape_width(s);
    if(h == 0)
        return 0;
    return w / h;
}

double shape_density(Shape *s)
{
    int area = shape_area(s);
    if(area == 0)
        return 0;
    return (double)s->count / area;
}


void free_shape(Shape *s)
{
    free(s->pixels);
    free(s);
}

typedef struct { int x, y; } Coord;

Shape **get_all_shape(Image* img)
{
    // capacity of result alloc
    int capacity = 4;

    int count = 0;

    Shape **res = malloc(sizeof (Shape*) * (capacity + 1));
    if (!res)
    {
        printf("Cannot allocate memory for the shape\n");
        exit(EXIT_FAILURE);
    }

    // foreach pixels
    for (int y = 0; y < img->height; ++y)
    {
        for (int x = 0; x < img->width; x++)
        {
            Pixel* p = get_pixel(img, x, y);
            if (p->r < 128 && p->isInShape == 0)
            {
                Shape *new_shape = create_shape();

                // stack capacity
                int stack_capacity = 4;
                int stack_count = 0;
                Coord *stack = malloc(sizeof(Coord) * stack_capacity);

                if (!stack)
                {
                    printf("Cannot allocate memory for the stack\n");
                    exit(EXIT_FAILURE);
                }
                stack[stack_count++] = (Coord){x, y};

                while(stack_count > 0)
                {
                    // if not enough capacity in malloc of stack realloc with capacity * 2
                    if (stack_count +1 >= stack_capacity)
                    {
                        stack_capacity *=2;
                        stack = realloc(stack, sizeof(Coord)* stack_capacity);
                        if (!stack)
                        {
                            printf("Cannot realloc memory for the shape\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    Coord c = stack[--stack_count];
                    int _x = c.x;
                    int _y = c.y;
                    Pixel *pixel = get_pixel(img, _x, _y);
                    if (pixel->r < 128 && pixel->isInShape == 0)
                    {
                        shape_add_pixel(new_shape, pixel);

                        // check all neighbours pixels
                        int dx[4] = {0, 1, 0, -1};
                        int dy[4] = {-1, 0, 1, 0};

                        for (int i = 0; i < 4; i++)
                        {
                            int nx = _x + dx[i];
                            int ny = _y + dy[i];
                            if(nx >= 0 && ny >= 0 && nx < img->width && ny < img->height)
                            {
                                Pixel *npx = get_pixel(img, nx, ny);
                                if(npx->r < 128 && npx->isInShape == 0)
                                {
                                    stack[stack_count++] = (Coord){nx, ny};
                                }
                            }
                        }
                    }
                }

                free(stack);

                // if the malloc for res is not enough realloc with capacity * 2
                if(count +1 >= capacity)
                {
                    capacity *=2;
                    res = realloc(res, sizeof(Shape*) * (capacity + 1));
                    if (!res)
                    {
                        printf("Cannot realloc memory for the shape\n");
                        exit(EXIT_FAILURE);
                    }
                }
                res[count++] = new_shape;
            }
        }
    }
    res[count] = NULL;
    return res;
}

void remove_small_shape(Image *img, Shape **shapes, int threshold)
{
    for (int i = 0; shapes[i] != NULL; ++i)
    {
        if (shapes[i]->count < threshold)
        {
            image_remove_shape(img, shapes[i]);
        }
    }
}
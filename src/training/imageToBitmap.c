#include <SLD2.h>
#include <stdio.h>

void import_image(const char* path)
{
    SDL_Surface *surface = IMG_Load(path);
    printf("%d\n", surface->pitch);
}


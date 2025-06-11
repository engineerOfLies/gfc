#include "gfc_types.h"

#include <string.h>
#include <stdio.h>

#include "simple_logger.h"

size_t get_file_Size(FILE *file)
{
    size_t size = 0;

    if (!file)
    {
        return 0;
    }
    do
    {
        fgetc(file);
    } while (!feof(file));
    size = ftell(file);
    rewind(file);
    return size;
}

float gfc_random_seeded(Uint32 seed)
{
    srand(seed);
    return gfc_random();
}

int gfc_random_int(int range)
{
    if (!range)return 0;
    return (rand()%range);
}

SDL_Rect gfc_sdl_rect(Sint32 x,Sint32 y,Uint32 w, Uint32 h)
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}

void *gfc_allocate_array(size_t typeSize,size_t count)
{
    void *array;
    if (count <= 0)
    {
        slog("cannot allocate zero elements");
        return NULL;
    }
    if (typeSize <= 0)
    {
        slog("cannot initialize an array of elements with zero size");
        return NULL;
    }
    array = malloc(typeSize * count);
    if (!array)
    {
        slog("failed to allocation space for %i elements of size %i",count,typeSize);
        return NULL;
    }
    memset(array,0,typeSize*count);
    return array;
}

/*eol@eof*/

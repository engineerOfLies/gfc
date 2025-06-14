#ifndef __GFC_TYPES_H__
#define __GFC_TYPES_H__

#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define GFC_EPSILON   1e-6f
#define GFC_ROOT2  1.414213562

#define GFC_HALF_PI 1.570796327
#define GFC_PI 3.141592654
#define GFC_PI_HALFPI 4.712388981
#define GFC_2PI 6.283185308

/* conversion factor for converting from radians to degrees*/
#define GFC_RADTODEG  57.295779513082

/* conversion factor for converting from degrees to radians*/
#define GFC_DEGTORAD  0.017453292519943295769236907684886

typedef short int Bool;

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

/**
 * @brief generate a random float (0 -1) based on the provided seed
 * @param seed the seed for the random number
 */
float gfc_random_seeded(Uint32 seed);

/**
 * @brief random macro taken from Id Software's Quake 2 Source.
 * This macro exposes the more random bits from the rand() function
 * @return a random float between 0 and 1.0
 */
#define gfc_random()  ((float)(rand ()%1000)/(float)1000.0)

/**
 * @brief random macro taken from Id Software's Quake 2 Source.
 * This macro exposes the more random bits from the rand() function
 * @return a random float between -1.0 and 1.0
 */
#define gfc_crandom() (((float)((rand()%1000)/(float)1000.0) * 2.0) - 1.0)

/**
 * @brief Gives a random integer value between 0 and the provided range
 * @note if range is negative, result should be 0 or negative
 * @return a random number between 0 and range.  
 * @note for floats, use gfc_random() or gfc_crandom()
 */
int gfc_random_int(int range);

/**
 * basic operations
 */
#ifndef MIN
#define MIN(a,b)          (a<=b?a:b)
#endif

#ifndef MAX
#define MAX(a,b)          (a>=b?a:b)
#endif

/**
 * @brief helper function to allocate and initialize arrays.  The array returned needs to be free'd
 * @param typeSize the size of the type for the array
 * @param count the number of elements in the array
 * @returns NULL on error (check logs), an array allocated and initialized to zero otherwise
 */
void *gfc_allocate_array(size_t typeSize,size_t count);

/**
 * @brief get the size of a file in byte (chars)
 * @param file the file to take a look at
 * @return the size of the file in bytes
 */
size_t get_file_Size(FILE *file);

#if defined(WIN32)
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#endif

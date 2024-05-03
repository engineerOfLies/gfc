#ifndef __GFC_NOISE_H__
#define __GFC_NOISE_H__

#include "gfc_vector.h"

/**
 * @brief generate a perline noise value at the vector position
 * @param in the input vector
 * @return a gradient value
 */
float gfc_perlin(Vector2D in);

#endif

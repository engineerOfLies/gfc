#ifndef __GFC_NOISE_H__
#define __GFC_NOISE_H__

#include "gfc_vector.h"

/**
 * @brief generate a perline noise value at the gfc_vector position
 * @param in the input gfc_vector
 * @return a gradient value
 */
float gfc_perlin(GFC_Vector2D in);

#endif

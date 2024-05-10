#ifndef __GFC_CONFIG_H__
#define __GFC_CONFIG_H__

#include <simple_json.h>

#include "gfc_vector.h"
#include "gfc_color.h"

/**
 * @brief extract a gfc_vector from json
 */
int sj_value_as_vector2d(SJson *json,GFC_Vector2D *output);
int sj_value_as_vector3d(SJson *json,GFC_Vector3D *output);
int sj_value_as_vector4d(SJson *json,GFC_Vector4D *output);

/**
 * @brief convert a gfc_vector to json
 */
SJson *sj_vector2d_new(GFC_Vector2D input);
SJson *sj_vector3d_new(GFC_Vector3D input);
SJson *sj_vector4d_new(GFC_Vector4D input);

/**
 * @brief get a gfc_vector from an object by its key
 */
int sj_object_get_vector2d(SJson *json,const char *key,GFC_Vector2D *output);
int sj_object_get_vector3d(SJson *json,const char *key,GFC_Vector3D *output);
int sj_object_get_vector4d(SJson *json,const char *key,GFC_Vector4D *output);

/**
 * @brief parse a color from json
 * @param value the value contianing a color gfc_vector
 * @return pure solid white, unless a color was successfully parsed
 */
GFC_Color sj_value_as_color(SJson *value);

/**
 * @brief get the object key as a color, defaulting to all zero if not found
 */
GFC_Color sj_object_get_color(SJson *json,const char *key);

#endif

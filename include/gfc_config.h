#ifndef __GFC_CONFIG_H__
#define __GFC_CONFIG_H__

#include <simple_json.h>

#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_string.h"
#include "gfc_text.h"
#include "gfc_color.h"
/**
 * @brief get a text word/line/block from the json value
 * @param json the json string to extract from
 * @param text [output] if found this will be populated, otherwise left untouched
 * @return 0 if not found or other error, 1 if it was found and the text populated
 */
int sj_value_as_textword(SJson *json,GFC_TextWord text);
int sj_value_as_textline(SJson *json,GFC_TextLine text);
int sj_value_as_textblock(SJson *json,GFC_TextBlock text);

/**
 * @brief get a text word/line/block from the json object by key
 * @param json the json object to extract from
 * @param key the key to get
 * @param text [output] if found this will be populated, otherwise left untouched
 * @return 0 if not found or other error, 1 if it was found and the text populated
 */
int sj_object_word_value(SJson *json,const char *key,GFC_TextWord text);
int sj_object_line_value(SJson *json,const char *key,GFC_TextLine text);
int sj_object_block_value(SJson *json,const char *key,GFC_TextBlock text);

/**
 * @brief extract a gfc_string from json
 * @param json the json to extract from
 * @return NULL on not found or error, a pointer to a created string otherwise
 * @note the returned string must be freed with gfc_string_free();
 */
GFC_String *sj_value_as_gfc_string(SJson *json);

/**
 * @brief extract a gfc_string from json object by its key
 * @param json the json object to extract from
 * @param key the key to extract
 * @return NULL on not found or error, a pointer to a created string otherwise
 * @note the returned string must be freed with gfc_string_free();
 */
GFC_String *sj_object_get_gfc_string(SJson *json,const char *key);

/**
 * @brief convert a gfc_string to a json string
 * @param string the gfc_string to convert
 * @return NULL on error, or a json value containing the text from the string
 */
SJson *sj_gfc_string_new(GFC_String *string);


/**
 * @brief extract a gfc_vector from json
 * @param json the json to extract from
 * @param output a pointer to the vector to store the values.
 * @return 0 if it failed, 1 if successful
 */
int sj_value_as_vector2d(SJson *json,GFC_Vector2D *output);
int sj_value_as_vector3d(SJson *json,GFC_Vector3D *output);
int sj_value_as_vector4d(SJson *json,GFC_Vector4D *output);

/**
 * @brief convert a gfc_vector to json
 * @param input the vector to convert
 * @return NULL on failure, a json array containing the vector as floats otherwise
 */
SJson *sj_vector2d_new(GFC_Vector2D input);
SJson *sj_vector3d_new(GFC_Vector3D input);
SJson *sj_vector4d_new(GFC_Vector4D input);

/**
 * @brief get a gfc_vector from an object by its key
 * @param 
 */
int sj_object_get_vector2d(SJson *json,const char *key,GFC_Vector2D *output);
int sj_object_get_vector3d(SJson *json,const char *key,GFC_Vector3D *output);
int sj_object_get_vector4d(SJson *json,const char *key,GFC_Vector4D *output);

/**
 * @brief convert a matrix into json for storing
 * @note matrix is stored in row major order as two arrays of floats
 * @param input the matrix to convert
 * @return NULL if something went wrong, the json containing a matrix otherwise
 */

SJson *sj_matrix4_new(GFC_Matrix4 input);
SJson *sj_matrix3_new(GFC_Matrix3 input);
SJson *sj_matrix2_new(GFC_Matrix2 input);

/**
 * @brief get matrix information from json
 * @param matrix the json containing a 2d array of floats
 * @param output the matrix that will be populated by the json
 * @return 0 if the data doesn't match expected or other error.  1 if it was successful
 */
int sj_value_as_matrix4(SJson *matrix,GFC_Matrix4 output);
int sj_value_as_matrix3(SJson *matrix,GFC_Matrix3 output);
int sj_value_as_matrix2(SJson *matrix,GFC_Matrix2 output);

/**
 * @brief get matrix data from an object by its key
 * @param json the object json to extract from
 * @param key the name fo the key to grab
 * @param output the matrix that will be populated by the json
 * @return 0 if it failed, 1 if it succeeded
 */
int sj_object_get_matrix4(SJson *json,const char *key,GFC_Matrix4 output);
int sj_object_get_matrix3(SJson *json,const char *key,GFC_Matrix3 output);
int sj_object_get_matrix2(SJson *json,const char *key,GFC_Matrix2 output);

/**
 * @brief extract a matrix based on component vectors provided in the json
 * @note keys in the object should be named "position","rotation", and "scale"
 * any not provided will default to identity
 * @note rotation is in degrees not radians
 * @param json the object json to extract from
 * @param key the name fo the key to grab
 * @param output the matrix that will be populated by the json
 * @return 0 if it failed, 1 if it succeeded
 */
int sj_object_get_matrix4_vectors(SJson *json,const char *key,GFC_Matrix4 output);

/**
 * @brief extract a matrix based on component vectors provided in the json object
 * @note keys in the object should be named "position","rotation", and "scale"
 * any not provided will default to identity
 * @note rotation is in degrees not radians
 * @param matrix the json containing a 3 vectors
 * @param output the matrix that will be populated by the json
 * @return 0 if it failed, 1 if it succeeded
 */
int sj_value_as_matrix4_vectors(SJson *matrix,GFC_Matrix4 output);


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

/**
 * @brief get the object key as a color / populating a pointer to one
 * @param json the json object containing a color
 * @param key the key for the color value
 * @param color [output] no op if not provided
 * @return 1 if the color was found and populated, 0 otherwise
 */
int sj_object_get_color_value(SJson *json,const char *key,GFC_Color *color);

/**
 * @brief convert a color to json format
 * @note all color will be stored in [r8,g8,b8,a8] format
 * @param input the color to convert
 * @return NULL on error, or json containin color data
 */
SJson *sj_color_new(GFC_Color input);

#endif

#ifndef __GFC_DECODE_H__
#define __GFC_DECODE_H__

#include "simple_json.h"

#include "gfc_types.h"

/**
 * @purpose this class is for decoding and encoding data in different ways
 */


/**
 * @brief extract json from a file that has been encoded into base64
 * @param filename a file containing base64 encoded json
 * @return NULL on error, or parsed json that must be sj_free()'d
 */

SJson *gfc_decode_json_file(const char *filename);

/**
 * @brief save json as a base64 encoded file instead of plaintext
 */
void gfc_encode_json_to_file(SJson *json, const char *filename);

/**
 * @brief convert a plaintext json file to a base64 encoded json file
 * @param inFilename the plaintext json file to convert
 * @param outFilename the base64 encoded file
 */
void gfc_decode_convert_json_file(const char *inFilename, const char *outFilename);

/**
 * @brief convert a base64 encoded json file to a plaintext json file
 * @param inFilename the base64 json file to convert
 * @param outFilename the plaintext json file
 */
void gfc_decode_extract_json_file(const char *inFilename, const char *outFilename);

/**
 * @brief decode a string from base64
 * @param in the input buffer
 * @param inLen the size of the input budder
 * @param outLen [output] the sizeof the output buffer
 * @return NULL on error, or the buffer created contain the decoded data buffer
 * @note any returned buffers must be free()'d
 * @source https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 * ACCESSED: 15/9/2022
 */
char *gfc_base64_decode (const char *in, size_t inLen, size_t *outLen);

/**
 * @brief encode a string into base64
 * @param input the input buffer
 * @param inputLength the size of the input budder
 * @param newSize [output] the sizeof the output buffer
 * @return NULL on error, or the buffer created contain the encoded data buffer
 * @note any returned buffers must be free()'d
 * @source https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 * ACCESSED: 15/9/2022
 */
char *gfc_base64_encode(const void* input, size_t inputLength, size_t *newSize);


#endif

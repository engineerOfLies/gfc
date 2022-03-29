#ifndef __GFC_HASHMAP_H__
#define __GFC_HASHMAP_H__

/**
 * gfc_hashmap
 * @license The MIT License (MIT)
   @copyright Copyright (c) 2022 EngineerOfLies
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "gfc_text.h"
#include "gfc_types.h"
#include "gfc_list.h"


typedef struct
{
    TextLine key;
    Uint32 hashValue;
    void *data;
}HashElement;

typedef struct
{
    List* map;      /**<the hash values*/
    Uint32 size;    /**<how many values are available in the hash*/
    Uint32 seed;    /**<the seed to calculate the hashed*/
}HashMap;

/**
 * @brief allocate and initialize an empty hashmap
 * @returns NULL on error or an empty hashmap otherwise
 * @note must be freed with gfc_hashmap_free();
 */
HashMap *gfc_hashmap_new();

/**
 * @brief free a previously allocated hashmap
 * @param map the hashmap to free
 */
void gfc_hashmap_free(HashMap *map);

/**
 * @brief add data to the hashmap
 * @param map the map to add a value to
 * @param key the key to retreive the data with
 * @param data the data to keep track of
 */
void gfc_hashmap_insert(HashMap *map,const char *key,void *data);

/**
 * @brief search the hashmap for the given key
 * @param map the map to searsh
 * @param key the value to key to search by
 */
void *gfc_hashmap_get(HashMap *map,const char *key);

/**
 * @brief delete a value out of the hashmap
 * @param map the map to delete a value from
 * @param key the key to the value to be deleted
 */
void gfc_hashmap_delete_by_key(HashMap *map,const char *key);


/**
 * @brief simple log the hash keys of the provided hashmap
 * @param map the map to print.  If NULL, this is a no op
 */
void gfc_hashmap_slog(HashMap *map);

#endif

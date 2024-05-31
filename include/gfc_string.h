#ifndef __GFC_STRING_H__
#define __GFC_STRING_H__

typedef struct
{
    char   *buffer; /**<contains the character data*/
    size_t  length; /**<how long the character array is*/
    size_t  size;   /**<how much data has been allocated*/
}GFC_String;


/**
 * @brief allocate an empty string
 * @return NULL if memory is full, a pointer to a GFC_String otherwise
 */
GFC_String *gfc_string_new();

/**
 * @brief allocate an empty string, pre-allocatign a set buffer length
 * @param size how many characters you want to pre-allocate.  if zero, this is a no-op
 * @return NULL if memory is full, a pointer to an empty GFC_String otherwise of size
 */
GFC_String *gfc_string_new_size(Uint32 size);


/**
 * @brief free a previously new'd string
 * @param string the string to free
 */
void gfc_string_free(GFC_String *string);

/**
 * @brief create a new string presetting it to the size needed and setting the buffer to the provided character
 * @param text the text to save into this string, if NULL, identical to gfc_string_new();
 * @return NULL on error, your created string otherwise
 */
GFC_String *gfc_string(const char *text);

/**
 * @brief create a new string presetting it to the size needed and setting the format buffer provided.  Similar to printf
 * @param text the text to save into this string, if NULL, identical to gfc_string_new();
 * @param ... the variables to populate the text format
 * @return NULL on error, your created string otherwise
 */
GFC_String *gfc_stringf(const char * text,...);

/**
 * @brief get a pointer to the character buffer in the string as a standard C str.
 * @param string the string to get from
 * @return NULL if not set, or the character array otherwise
 */
const char *gfc_string_text(GFC_String *string);

/**
 * @brief append the text to the end of the string, expanding as necessary
 * @param string the string to add to
 * @param text the text to add to it
 */
void gfc_string_append(GFC_String *string,const char *text);

/**
 * @brief append to the string a formated string
 * @param string the string to append to
 * @param format the format text to append
 * @param ... the variables used to populate the formatted text
 */
void gfc_string_appendf(GFC_String *string,const char *format,...);

/**
 * @brief add the text from one string to the end of another
 * @param string the string to add to
 * @param add the string to add from
 */
void gfc_string_concat(GFC_String *string,const GFC_String *add);

/**
 * @brief append the text to the start of the string, expanding as necessary
 * @param string the string to add to
 * @param text the text to add to it
 */
void gfc_string_prepend(GFC_String *string,const char *text);

/**
 * @brief append the text to the start of the string, expanding as necessary
 * @param string the string to add to
 * @param text the text to add to it formatted like a printf format string
 * @param ... the variables used to populate the formatted text
 */
void gfc_string_prependf(GFC_String *string,const char *text,...);

/**
 * @brief perform a string compare against the two strings
 * @param string1 one string to check
 * @param string2 second string to check
 * @return -3 if there are any NULL values, otherwise the results a strcmp on the two buffers
 */
int gfc_string_cmp(GFC_String *string1,GFC_String *string2);

/**
 * @brief perform a string compare against the two strings, also validating length
 * @param string1 one string to check
 * @param string2 second string to check
 * @return -3 if there are any NULL values, -2 if the strings are different length, otherwise the results a strcmp on the two buffers
 */
int gfc_string_l_cmp(GFC_String *string1,GFC_String *string2);

/**
 * @brief perform a string compare against a string and char buffer
 * @param string1 one string to check
 * @param string2 second string to check
 * @return -3 if there are any NULL values, otherwise the results a strcmp on the two buffers
 */
int gfc_string_strcmp(GFC_String *string1,const char *string2);

/**
 * @brief perform a string compare against a string and char buffer, also validating length
 * @param string1 one string to check
 * @param string2 second string to check
 * @return -3 if there are any NULL values, -2 if the strings are different length, otherwise the results a strcmp on the two buffers
 */
int gfc_string_l_strcmp(GFC_String *string1,const char *string2);

#endif

#include <stdarg.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_string.h"

#define GFC_STRING_DEFAULT_LENGTH 256

GFC_String *gfc_string_new()
{
    return gfc_string_new_size(GFC_STRING_DEFAULT_LENGTH);
}

GFC_String *gfc_string_new_size(Uint32 size)
{
    GFC_String *string;
    if (!size)return NULL;
    string = gfc_allocate_array(sizeof(GFC_String),1);
    if (!string)
    {
        slog("failed to allocate memory for a new GFC_String");
        return NULL;
    }
    string->buffer = gfc_allocate_array(sizeof(char),size);
    if (!string->buffer)
    {
        slog("failed to allocate memory for new GFC_String buffer");
        gfc_string_free(string);
        return NULL;
    }
    string->size = size;
    return string;
}


void gfc_string_free(GFC_String *string)
{
    if (!string)return;
    if (string->buffer)
    {
        free(string->buffer);
    }
    free(string);
}

GFC_String *gfc_string(const char *text)
{
    GFC_String *string;
    Uint32 length,size;
    if (!text)return gfc_string_new();
    length = strlen(text);
    if (!length)return gfc_string_new();
    for (size = GFC_STRING_DEFAULT_LENGTH;length > size;size*=2);//find the needed buffer size for the string length
    string = gfc_string_new_size(size);
    if (!string)return NULL;
    strcpy(string->buffer,text);
    string->length = length;
    return string;
}

char *gfc_string_get_formated_text(size_t *sizeOut,const char *format,va_list ap)
{
    int r;
    size_t size;
    char *buffer;
    buffer = gfc_allocate_array(sizeof(char),GFC_STRING_DEFAULT_LENGTH);
    if (!buffer)return NULL;
    size = GFC_STRING_DEFAULT_LENGTH;
    while ((r = vsprintf(buffer,format,ap)) < 0)
    {
        size *= 2;
        if (buffer)free(buffer);
        buffer = gfc_allocate_array(sizeof(char),size);
        if (!buffer)
        {
            return NULL;
        }
    }
    if (sizeOut)*sizeOut = size;
    return buffer;
}

GFC_String *gfc_stringf(const char * text,...)
{
    va_list ap;
    size_t size = GFC_STRING_DEFAULT_LENGTH;
    char *buffer;
    GFC_String *string;
    if (!text)return gfc_string_new();
    buffer = gfc_allocate_array(sizeof(char),size);
    if (!buffer)return NULL;
    va_start(ap,text);
    while (vsprintf(buffer,text,ap) < 0)
    {
        size *= 2;
        if (buffer)free(buffer);
        buffer = gfc_allocate_array(sizeof(char),size);
        if (!buffer)
        {
            return NULL;
        }
        va_end(ap);
        va_start(ap,text);
    }
    va_end(ap);
    string = gfc_allocate_array(sizeof(GFC_String),1);
    if (!string)
    {
        slog("failed to allocated memory for GFC_String");
        free(buffer);
        return NULL;
    }
    string->buffer = buffer;
    string->size = size;
    string->length = strlen(buffer);
    return string;
}

const char *gfc_string_text(GFC_String *string)
{
    if (!string)return NULL;
    return string->buffer;
}

void gfc_string_append(GFC_String *string,const char *text)
{
    char *buffer;
    size_t newlength,size;
    if ((!string)||(!text))return;
    newlength = strlen(text) + string->length;
    for (size = string->size;newlength > size;size*=2);//make sure we can fit
    if (size > string->size)
    {
        //we need to expand;
        buffer = gfc_allocate_array(sizeof(char),size);
        sprintf(buffer,"%s%s",string->buffer,text);
        free(string->buffer);
        string->buffer = buffer;
        string->length = newlength;
        string->size = size;
        return;
    }
    buffer = &string->buffer[string->length];
    strcpy(buffer,text);
    string->length = newlength;
}

void gfc_string_appendf(GFC_String *string,const char *format,...)
{
    va_list ap;
    size_t size = GFC_STRING_DEFAULT_LENGTH;
    char *buffer;
    if (!format)return;
    buffer = gfc_allocate_array(sizeof(char),size);
    if (!buffer)return;
    va_start(ap,format);
    while (vsprintf(buffer,format,ap) < 0)
    {
        size *= 2;
        if (buffer)free(buffer);
        buffer = gfc_allocate_array(sizeof(char),size);
        if (!buffer)
        {
            return;
        }
        va_end(ap);
        va_start(ap,format);
    }
    va_end(ap);
    gfc_string_append(string,buffer);
    free(buffer);
}

void gfc_string_concat(GFC_String *string,const GFC_String *add)
{
    if ((!string)||(!add)||(!string->buffer)||(!add->buffer))return;
    gfc_string_append(string,add->buffer);
}

void gfc_string_prepend(GFC_String *string,const char *text)
{
    char *buffer;
    size_t newlength,size;
    if ((!string)||(!text))return;
    newlength = strlen(text) + string->length;
    for (size = string->size;newlength > size;size*=2);//make sure we can fit
    buffer = gfc_allocate_array(sizeof(char),size);
    sprintf(buffer,"%s%s",text,string->buffer);
    free(string->buffer);
    string->buffer = buffer;
    string->length = newlength;
    string->size = size;
}

void gfc_string_prependf(GFC_String *string,const char *format,...)
{
    va_list ap;
    size_t size = GFC_STRING_DEFAULT_LENGTH;
    char *buffer;
    if (!format)return;
    buffer = gfc_allocate_array(sizeof(char),size);
    if (!buffer)return;
    va_start(ap,format);
    while (vsprintf(buffer,format,ap) < 0)
    {
        size *= 2;
        if (buffer)free(buffer);
        buffer = gfc_allocate_array(sizeof(char),size);
        if (!buffer)
        {
            return;
        }
        va_end(ap);
        va_start(ap,format);
    }
    va_end(ap);
    gfc_string_prepend(string,buffer);
    free(buffer);
}

int gfc_string_strcmp(GFC_String *string1,const char *string2)
{
    if ((!string1)||(!string2)||(!string1->buffer))return -3;
    return strcmp(string1->buffer,string2);
}

int gfc_string_l_strcmp(GFC_String *string1,const char *string2)
{
    if ((!string1)||(!string2)||(!string1->buffer))return -3;
    return gfc_strlcmp(string1->buffer,string2);
}

int gfc_string_cmp(GFC_String *string1,GFC_String *string2)
{
    if ((!string1)||(!string2)||(!string1->buffer)||(!string2->buffer))return -3;
    return strcmp(string1->buffer,string2->buffer);
}

int gfc_string_l_cmp(GFC_String *string1,GFC_String *string2)
{
    if ((!string1)||(!string2)||(!string1->buffer)||(!string2->buffer))return -3;
    return gfc_strlcmp(string1->buffer,string2->buffer);
}

/*eol@eof*/

#include "simple_logger.h"

#include "simple_json_parse.h"

#include "gfc_decode.h"

SJson *gfc_decode_json_file(const char *filename)
{
    FILE *file;
    SJson *json;
    long size,read;
    char *buffer = NULL;
    char *decodedBuffer = NULL;
    size_t decodedBufferSize = 0;
    
    file = fopen(filename,"r");
    if (!file)
    {
        slog("failed to open file %s",filename);
        return NULL;
    }
    size = get_file_Size(file);
    if (size <= 0)
    {
        slog("error getting file size for %s",filename);
        fclose(file);
        return NULL;
    }
    buffer = gfc_allocate_array(sizeof(char),(size + 2));
    
    if (buffer == NULL)
    {
        slog("failed to allocate character buffer for json file %s",filename);
        fclose(file);
        return NULL;
    }
    
    if ((read = fread(buffer, sizeof(char), size, file)) != size)
    {
        slog("expected to read %li characters, but read %li instead\n for file %s",size,read,filename);
    }
    fclose(file);
    
    decodedBuffer = gfc_base64_decode (buffer, size, &decodedBufferSize);
    if (!decodedBuffer)
    {
        slog("failed to decode json from base64 file %s",filename);
        free(buffer);
        return NULL;
    }
    free(buffer);
    
    json = sj_parse_buffer(decodedBuffer,read);
    if (!json)
    {
        slog("file %s failed to parse\n",filename);
    }
    free(decodedBuffer);
    return json;
}

void gfc_encode_json_to_file(SJson *json, const char *filename)
{
    FILE *file;
    SJString *string;
    char *buffer = NULL;//after encoding;
    size_t bufferSize = 0;
    if ((!json)||(!filename))
    {
        slog("failed to convert json to base64 file, missing parameters");
        return;
    }
    if ((!json) || (!json->get_string))return;
    string = json->get_string(json);
    if (!string)return;
    file = fopen(filename,"w");
    if (!file)
    {
        sj_string_free(string);
        slog("failed to open %s for writing",filename);
        return;
    }
    buffer = gfc_base64_encode(string->text, strlen(string->text), &bufferSize);
    if (!buffer)
    {
        sj_string_free(string);
        slog("failed to encode json file to base64 for %s",filename);
        fclose(file);
        return;
    }
    fwrite(buffer,bufferSize,1,file);
    free(buffer);
    sj_string_free(string);
    fclose(file);
}

void gfc_decode_convert_json_file(const char *inFilename, const char *outFilename)
{
    SJson *json;
    if ((!inFilename)||(!outFilename))
    {
        slog("gfc_decode_convert_json_file: missing parameters");
        return;
    }
    json = sj_load(inFilename);
    if (!json)return;
    gfc_encode_json_to_file(json, outFilename);
    sj_free(json);
}

void gfc_decode_extract_json_file(const char *inFilename, const char *outFilename)
{
    SJson *json;
    if ((!inFilename)||(!outFilename))
    {
        slog("gfc_decode_convert_json_file: missing parameters");
        return;
    }
    json = gfc_decode_json_file(inFilename);
    if (!json)return;
    sj_save(json,outFilename);
    sj_free(json);
}


char *gfc_base64_encode(const void* input, size_t inputLength, size_t *newSize)
{
    const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *result;
    const uint8_t *data = (const uint8_t *)input;
    size_t resultSize = 0;
    size_t resultIndex = 0;
    size_t x;
    uint32_t n = 0;
    int padCount = inputLength % 3;
    uint8_t n0, n1, n2, n3;

    resultSize = inputLength + (inputLength/2) + 1;
    if (!input)return NULL;
    if (!resultSize) return NULL;
    result = gfc_allocate_array(sizeof(char),resultSize);
    if (!result)return NULL;
   
   /* increment over the length of the string, three characters at a time */
    for (x = 0; x < inputLength; x += 3) 
    {
        /* these three 8-bit (ASCII) characters become one 24-bit number */
        n = ((uint32_t)data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
        
        if((x+1) < inputLength)
            n += ((uint32_t)data[x+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
        
        if((x+2) < inputLength)
            n += data[x+2];

        /* this 24-bit number gets separated into four 6-bit numbers */
        n0 = (uint8_t)(n >> 18) & 63;
        n1 = (uint8_t)(n >> 12) & 63;
        n2 = (uint8_t)(n >> 6) & 63;
        n3 = (uint8_t)n & 63;
            
        /*
        * if we have one byte available, then its encoding is spread
        * out over two characters
        */
        if(resultIndex >= resultSize)
        {
            free(result);
            return NULL;   /* indicate failure: buffer too small */
        }
        result[resultIndex++] = base64chars[n0];
        if(resultIndex >= resultSize)
        {
            free(result);
            return NULL;   /* indicate failure: buffer too small */
        }
        result[resultIndex++] = base64chars[n1];

        /*
        * if we have only two bytes available, then their encoding is
        * spread out over three chars
        */
        if((x+1) < inputLength)
        {
            if(resultIndex >= resultSize)
            {
                free(result);
                return NULL;   /* indicate failure: buffer too small */
            }
            result[resultIndex++] = base64chars[n2];
        }

        /*
        * if we have all three bytes available, then their encoding is spread
        * out over four characters
        */
        if((x+2) < inputLength)
        {
            if(resultIndex >= resultSize)
            {
                free(result);
                return NULL;   /* indicate failure: buffer too small */
            }
            result[resultIndex++] = base64chars[n3];
        }
    }

    /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
    if (padCount > 0) 
    { 
        for (; padCount < 3; padCount++) 
        { 
            if(resultIndex >= resultSize)
            {
                free(result);
                return NULL;   /* indicate failure: buffer too small */
            }
            result[resultIndex++] = '=';
        } 
    }
    if(resultIndex >= resultSize)
    {
        free(result);
        return NULL;   /* indicate failure: buffer too small */
    }
    result[resultIndex] = 0;
    
    if (newSize)*newSize = resultSize;
    return result;   /* indicate success */
}


#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

char *gfc_base64_decode (const char *in, size_t inLen, size_t *outLen)
{ 
    unsigned char *out,*outIt;
    const char *end = in + inLen;
    const char *it = in;
    size_t allocateSize;
    int iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    
    if (!in)
    {
        slog("no input data provided for decoding");
        return NULL;
    }
    if (!inLen)
    {
        slog("input length is zero, cannot decode");
        return NULL;
    }
    
    allocateSize = inLen;
    
    if (!allocateSize)
    {
        slog("output length is zero, cannot decode");
        return NULL;
    }
    out = gfc_allocate_array(sizeof(char),allocateSize);
    
    if (!out)
    {
        slog("failed to allocated output for decoding");
        return NULL;
    }
    outIt = out;
    
    while (it < end)
    {
        unsigned char c = d[(int)*it++];
        
        switch (c)
        {
            case WHITESPACE: 
                continue;   /* skip whitespace */
            case INVALID:
                slog("invalid character in base64 data, aborting");
                free(out);
                return NULL;   /* invalid input, return error */
            case EQUALS:    /* pad character, end of data */
                it = end;
                continue;
            default:
                buf = buf << 6 | c;
                iter++; // increment the number of iteration
                /* If the buffer is full, split it into bytes */
                if (iter >= 4)
                {
                    if ((len += 3) > allocateSize)
                    {
                        free(out);
                        return NULL; /* buffer overflow */
                    }
                    *(outIt++) = (buf >> 16) & 255;
                    *(outIt++) = (buf >> 8) & 255;
                    *(outIt++) = buf & 255;
                    buf = 0;
                    iter = 0;
                }   
        }
    }
   
    if (iter == 3)
    {
        if ((len += 2) > allocateSize)
        {
            slog("buffer overflow detected");
            free(out);
            return NULL; /* buffer overflow */
        }
        *(outIt++) = (buf >> 10) & 255;
        *(outIt++) = (buf >> 2) & 255;
    }
    else if (iter == 2)
    {
        if (++len > allocateSize)
        {
            free(out);
            return NULL; /* buffer overflow */
        }
        *(outIt++) = (buf >> 4) & 255;
    }
    if (outLen)*outLen = len; /* modify to reflect the actual output size */
    return (char *)out;
}

/*eol@eof*/

#include "simple_logger.h"
#include "gfc_config.h"


// string stuff

int sj_value_as_textword(SJson *json,GFC_TextWord text)
{
    const char *str;
    str = sj_get_string_value(json);
    if (!str)return 0;
    gfc_word_cpy(text,str);
    return 1;
}

int sj_value_as_textline(SJson *json,GFC_TextLine text)
{
    const char *str;
    str = sj_get_string_value(json);
    if (!str)return 0;
    gfc_line_cpy(text,str);
    return 1;
}

int sj_value_as_textblock(SJson *json,GFC_TextBlock text)
{
    const char *str;
    str = sj_get_string_value(json);
    if (!str)return 0;
    gfc_block_cpy(text,str);
    return 1;
}

int sj_object_word_value(SJson *json,const char *key,GFC_TextWord text)
{
    return sj_value_as_textword(sj_object_get_value(json,key),text);
}

int sj_object_line_value(SJson *json,const char *key,GFC_TextLine text)
{
    return sj_value_as_textline(sj_object_get_value(json,key),text);
}

int sj_object_block_value(SJson *json,const char *key,GFC_TextBlock text)
{
    return sj_value_as_textblock(sj_object_get_value(json,key),text);
}

GFC_String *sj_value_as_gfc_string(SJson *json)
{
    const char *text;
    text = sj_get_string_value(json);
    if (!text)return NULL;
    return gfc_string(text);
}

GFC_String *sj_object_get_gfc_string(SJson *json,const char *key)
{
    return sj_value_as_gfc_string(sj_object_get_value(json,key));
}

SJson *sj_gfc_string_new(GFC_String *string)
{
    if (!string)return NULL;
    return sj_new_str(string->buffer);
}


//matrix stuff

int sj_object_get_matrix4_vectors(SJson *json,const char *key,GFC_Matrix4 output)
{
    return sj_value_as_matrix4_vectors(sj_object_get_value(json,key),output);
}

int sj_value_as_matrix4_vectors(SJson *matrix,GFC_Matrix4 output)
{
    GFC_Vector3D position = {0};
    GFC_Vector3D rotation = {0};
    GFC_Vector3D scale = gfc_vector3d(1,1,1);
    if (!matrix)return 0;
    sj_object_get_vector3d(matrix,"position",&position);
    sj_object_get_vector3d(matrix,"rotation",&rotation);
    gfc_vector3d_scale(rotation,rotation,GFC_DEGTORAD);
    sj_object_get_vector3d(matrix,"scale",&scale);
    gfc_matrix4_from_vectors(output,position,rotation,scale);
    return 1;
}


int sj_object_get_matrix4(SJson *json,const char *key,GFC_Matrix4 output)
{
    return sj_value_as_matrix4(sj_object_get_value(json,key),output);
}

int sj_object_get_matrix3(SJson *json,const char *key,GFC_Matrix3 output)
{
    return sj_value_as_matrix3(sj_object_get_value(json,key),output);
}

int sj_object_get_matrix2(SJson *json,const char *key,GFC_Matrix2 output)
{
    return sj_value_as_matrix2(sj_object_get_value(json,key),output);
}

int sj_value_as_matrix4(SJson *matrix,GFC_Matrix4 output)
{
    int i,c;
    int j,d;
    GFC_Matrix4 temp;
    SJson *row;
    SJson *column;
    if (!matrix)return 0;
    c = sj_array_get_count(matrix);
    if (c != 4)return 0;//not the right size
    for (i = 0; i < c; i++)
    {
        row = sj_array_get_nth(matrix,i);
        if (!row)
        {
            return 0;
        }
        d = sj_array_get_count(row);
        if (d != 4)return 0;//not the right size
        for (j = 0;j < d;j++)
        {
            column = sj_array_get_nth(row,j);
            if (!column)return 0;//something is wrong
            sj_get_float_value(column,&temp[i][j]);
        }
    }
    gfc_matrix4_copy(output,temp);
    return 1;
}

int sj_value_as_matrix3(SJson *matrix,GFC_Matrix3 output)
{
    int i,c;
    int j,d;
    GFC_Matrix3 temp;
    SJson *row;
    SJson *column;
    if (!matrix)return 0;
    c = sj_array_get_count(matrix);
    if (c != 3)return 0;//not the right size
    for (i = 0; i < c; i++)
    {
        row = sj_array_get_nth(matrix,i);
        if (!row)
        {
            return 0;
        }
        d = sj_array_get_count(row);
        if (d != 3)return 0;//not the right size
        for (j = 0;j < d;j++)
        {
            column = sj_array_get_nth(row,j);
            if (!column)return 0;//something is wrong
            sj_get_float_value(column,&temp[i][j]);
        }
    }
    gfc_matrix3_copy(output,temp);
    return 1;
}

int sj_value_as_matrix2(SJson *matrix,GFC_Matrix2 output)
{
    int i,c;
    int j,d;
    GFC_Matrix2 temp;
    SJson *row;
    SJson *column;
    if (!matrix)return 0;
    c = sj_array_get_count(matrix);
    if (c != 2)return 0;//not the right size
    for (i = 0; i < c; i++)
    {
        row = sj_array_get_nth(matrix,i);
        if (!row)
        {
            return 0;
        }
        d = sj_array_get_count(row);
        if (d != 2)return 0;//not the right size
        for (j = 0;j < d;j++)
        {
            column = sj_array_get_nth(row,j);
            if (!column)return 0;//something is wrong
            sj_get_float_value(column,&temp[i][j]);
        }
    }
    gfc_matrix2_copy(output,temp);
    return 1;
}

SJson *sj_matrix4_new(GFC_Matrix4 input)
{
    int i;
    SJson *row,*matrix;
    
    matrix = sj_array_new();
    if (!matrix)return NULL;
    for (i = 0;i < 4;i++)
    {
        row = sj_array_new();
        if (!row)
        {
            sj_free(matrix);
            return NULL;
        }
        sj_array_append(row,sj_new_float(input[i][0]));
        sj_array_append(row,sj_new_float(input[i][1]));
        sj_array_append(row,sj_new_float(input[i][2]));
        sj_array_append(row,sj_new_float(input[i][3]));
        sj_array_append(matrix,row);
    }
    return matrix;
}

SJson *sj_matrix3_new(GFC_Matrix3 input)
{
    int i;
    SJson *row,*matrix;
    
    matrix = sj_array_new();
    if (!matrix)return NULL;
    for (i = 0;i < 3;i++)
    {
        row = sj_array_new();
        if (!row)
        {
            sj_free(matrix);
            return NULL;
        }
        sj_array_append(row,sj_new_float(input[i][0]));
        sj_array_append(row,sj_new_float(input[i][1]));
        sj_array_append(row,sj_new_float(input[i][2]));
        sj_array_append(matrix,row);
    }
    return matrix;
}

SJson *sj_matrix2_new(GFC_Matrix2 input)
{
    int i;
    SJson *row,*matrix;
    
    matrix = sj_array_new();
    if (!matrix)return NULL;
    for (i = 0;i < 2;i++)
    {
        row = sj_array_new();
        if (!row)
        {
            sj_free(matrix);
            return NULL;
        }
        sj_array_append(row,sj_new_float(input[i][0]));
        sj_array_append(row,sj_new_float(input[i][1]));
        sj_array_append(matrix,row);
    }
    return matrix;
}


// vector stuff

SJson *sj_vector2d_new(GFC_Vector2D input)
{
    SJson *json;
    
    json = sj_array_new();
    if (!json)return NULL;
    sj_array_append(json,sj_new_float(input.x));
    sj_array_append(json,sj_new_float(input.y));
    return json;
}

SJson *sj_vector3d_new(GFC_Vector3D input)
{
    SJson *json;
    
    json = sj_array_new();
    if (!json)return NULL;
    sj_array_append(json,sj_new_float(input.x));
    sj_array_append(json,sj_new_float(input.y));
    sj_array_append(json,sj_new_float(input.z));
    return json;
}

SJson *sj_vector4d_new(GFC_Vector4D input)
{
    SJson *json;
    
    json = sj_array_new();
    if (!json)return NULL;
    sj_array_append(json,sj_new_float(input.x));
    sj_array_append(json,sj_new_float(input.y));
    sj_array_append(json,sj_new_float(input.z));
    sj_array_append(json,sj_new_float(input.w));
    return json;
}

int sj_object_get_vector2d(SJson *json,const char *key,GFC_Vector2D *output)
{
    return sj_value_as_vector2d(sj_object_get_value(json,key),output);
}

int sj_object_get_vector3d(SJson *json,const char *key,GFC_Vector3D *output)
{
    return sj_value_as_vector3d(sj_object_get_value(json,key),output);
}

int sj_object_get_vector4d(SJson *json,const char *key,GFC_Vector4D *output)
{
    return sj_value_as_vector4d(sj_object_get_value(json,key),output);
}

int sj_value_as_vector2d(SJson *json,GFC_Vector2D *output)
{
    const char *text = NULL;
    float numbers[4];
    int i,count;
    SJson *value;
    if (!json)return 0;
    if (sj_is_array(json))
    {
        count = sj_array_get_count(json);
        if (count < 2)return 0;
        if (count > 2)count = 2;
        for (i = 0; i < count;i++)
        {
            value = sj_array_get_nth(json,i);
            sj_get_float_value(value,&numbers[i]);
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
        }
        return 1;
    }
    if (sj_is_string(json))
    {
        text = sj_get_string_value(json);
        if(sscanf(text,"%f,%f",&numbers[0],&numbers[1]) != 2)
        {
            return 0;
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
        }
        return 1;
    }
    return 0;
}

int sj_value_as_vector3d(SJson *json,GFC_Vector3D *output)
{
    const char *text = NULL;
    float numbers[4];
    int i,count;
    SJson *value;
    if (!json)return 0;
    if (sj_is_array(json))
    {
        count = sj_array_get_count(json);
        if (count < 3)return 0;
        if (count > 3)count = 3;
        for (i = 0; i < count;i++)
        {
            value = sj_array_get_nth(json,i);
            sj_get_float_value(value,&numbers[i]);
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
        }
        return 1;
    }
    if (sj_is_string(json))
    {
        text = sj_get_string_value(json);
        if(sscanf(text,"%f,%f,%f",&numbers[0],&numbers[1],&numbers[2]) != 3)
        {
            return 0;
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
        }
        return 1;
    }
    return 0;
}

int sj_value_as_vector4d(SJson *json,GFC_Vector4D *output)
{
    const char *text = NULL;
    float numbers[4];
    int i,count;
    SJson *value;
    if (!json)
    {
        return 0;
    }
    if (sj_is_array(json))
    {
        count = sj_array_get_count(json);
        if (count < 4)
        {
            slog("not enough elements in the array (%i) for a GFC_Vector4D",count);
            return 0;
        }
        if (count > 4)count = 4;
        for (i = 0; i < count;i++)
        {
            value = sj_array_get_nth(json,i);
            sj_get_float_value(value,&numbers[i]);
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
            output->w = numbers[3];
        }
        return 1;
    }
    if (sj_is_string(json))
    {
        text = sj_get_string_value(json);
        if(sscanf(text,"%f,%f,%f,%f",&numbers[0],&numbers[1],&numbers[2],&numbers[3]) != 4)
        {
            return 0;
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
            output->w = numbers[3];
        }
        return 1;
    }
    slog("json is not an array or string");
    sj_echo(json);
    return 0;
}

// color stuff

GFC_Color sj_object_get_color(SJson *json,const char *key)
{
    return sj_value_as_color(sj_object_get_value(json,key));
}

int sj_object_get_color_value(SJson *json,const char *key,GFC_Color *color)
{
    SJson *value;
    GFC_Vector4D colorv;
    if (!color)return 0;
    value = sj_object_get_value(json,key);
    if (!value)return 0;
    if (!sj_value_as_vector4d(value,&colorv))return 0;
    *color = gfc_color_from_vector4(colorv);
    return 1;
}

GFC_Color sj_value_as_color(SJson *value)
{
    GFC_Color color;
    GFC_Vector4D colorv = {255,255,255,255};
    color = gfc_color8(255,255,255,255);
    if (!value)return color;
    sj_value_as_vector4d(value,&colorv);
    color = gfc_color_from_vector4(colorv);
    return color;
}

SJson *sj_color_new(GFC_Color input)
{
    GFC_Color color;
    SJson *json;
    json = sj_array_new();
    if (!json)return NULL;
    color = gfc_color_to_int8(input);
    sj_array_append(json,sj_new_int(color.r));
    sj_array_append(json,sj_new_int(color.g));
    sj_array_append(json,sj_new_int(color.b));
    sj_array_append(json,sj_new_int(color.a));
    return json;
    
}
/*eol@eof*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "gfc_matrix.h"
#include "simple_logger.h"

/*
 * Code has been adapted from glm to C for this project
 */
GFC_Vector3D gfc_unproject(GFC_Vector3D in,GFC_Matrix4 view, GFC_Matrix4 proj,GFC_Vector2D viewport)
{
    GFC_Vector3D out = {0,0,0};
    GFC_Matrix4 InvProj = {0},InvView = {0};
    GFC_Vector4D tmp,obj,eye;
    
    if ((!viewport.x)||(!viewport.y))
    {
        slog("cannot unproject into a view of zero width or height");
        return out;
    }
    
    gfc_matrix4_invert(InvProj,proj);
    gfc_matrix4_invert(InvView,view);
    
    tmp.x =(2.0f*((in.x)/viewport.x))-1.0f,
    tmp.y =(2.0f*((in.y)/viewport.y))-1.0f,
    tmp.z = (in.z * 2) -1;
    tmp.w = 1;
        
    gfc_matrix4_multiply_v(
        &eye,
        InvView,
        tmp);

    gfc_matrix4_multiply_v(
        &obj,
        InvProj,
        eye);
    
    
    if (!obj.w)
    {
        slog("bad unprojection");
        return out;
    }
    out.x = obj.x/obj.w;
    out.y = obj.y/obj.w;
    out.z = obj.z/obj.w;
    
    return out;
}

void _gfc_matrix4_slog(const char *filename,Uint32 line,GFC_Matrix4 mat)
{
    _slog(filename,line,"%f,%f,%f,%f",mat[0][0],mat[0][1],mat[0][2],mat[0][3]);
    _slog(filename,line,"%f,%f,%f,%f",mat[1][0],mat[1][1],mat[1][2],mat[1][3]);
    _slog(filename,line,"%f,%f,%f,%f",mat[2][0],mat[2][1],mat[2][2],mat[2][3]);
    _slog(filename,line,"%f,%f,%f,%f",mat[3][0],mat[3][1],mat[3][2],mat[3][3]);
}

void _gfc_matrix3_slog(const char *filename,Uint32 line,GFC_Matrix3 mat)
{
    _slog(filename,line,"%f,%f,%f,%f",mat[0][0],mat[0][1],mat[0][2],mat[0][3]);
    _slog(filename,line,"%f,%f,%f,%f",mat[1][0],mat[1][1],mat[1][2],mat[1][3]);
    _slog(filename,line,"%f,%f,%f,%f",mat[2][0],mat[2][1],mat[2][2],mat[2][3]);
}

void _gfc_matrix2_slog(const char *filename,Uint32 line,GFC_Matrix2 mat)
{
    _slog(filename,line,"%f,%f,%f,%f",mat[0][0],mat[0][1],mat[0][2],mat[0][3]);
    _slog(filename,line,"%f,%f,%f,%f",mat[1][0],mat[1][1],mat[1][2],mat[1][3]);
}

void gfc_matrix4_to_vectors(
    GFC_Matrix4 in,
    GFC_Vector3D *translation,
    GFC_Vector3D *rotation,
    GFC_Vector3D *scale)
{
    if (scale)
    {
        scale->x = in[0][0];
        scale->y = in[1][1];
        scale->z = in[2][2];
    }
    if (translation)
    {
        translation->x = in[3][0];
        translation->y = in[3][1];
        translation->z = in[3][2];
    }
    if (rotation)
    {
        if ((in[0][0] == 1.0f)||(in[0][0] == -1.0f))
        {
            rotation->x = -atan2f(in[0][2], in[2][3]);
            rotation->z = 0;
            rotation->y = 0;

        }else 
        {

            rotation->x = -atan2(-in[2][0],in[0][0]);
            rotation->z = asin(in[1][0]);
            rotation->y = -atan2(-in[1][2],in[1][1]);
        }
    }
}

void gfc_matrix4_from_vectors(
    GFC_Matrix4 out,
    GFC_Vector3D translation,
    GFC_Vector3D rotation,
    GFC_Vector3D scale)
{
    gfc_matrix4_identity(out);
    
    gfc_matrix4_scale(out,out,scale);
    gfc_matrix4_rotate_by_vector(out,out,rotation);
    gfc_matrix4_translate(out,out,translation);
}

void gfc_matrix4_to_matrix16(float m16[16],GFC_Matrix4 m4)
{
    m16[0]  = m4[0][0];
    m16[1]  = m4[0][1];
    m16[2]  = m4[0][2];
    m16[3]  = m4[0][3];
    m16[4]  = m4[1][0];
    m16[5]  = m4[1][1];
    m16[6]  = m4[1][2];
    m16[7]  = m4[1][3];
    m16[8]  = m4[2][0];
    m16[9]  = m4[2][1];
    m16[10] = m4[2][2];
    m16[11] = m4[2][3];
    m16[12] = m4[3][0];
    m16[13] = m4[3][1];
    m16[14] = m4[3][2];
    m16[15] = m4[3][3];
}

void gfc_matrix16_to_matrix4(GFC_Matrix4 m4,float m16[16])
{
    m4[0][0] = m16[0]  ;
    m4[0][1] = m16[1]  ;
    m4[0][2] = m16[2]  ;
    m4[0][3] = m16[3]  ;
    m4[1][0] = m16[4]  ;
    m4[1][1] = m16[5]  ;
    m4[1][2] = m16[6]  ;
    m4[1][3] = m16[7]  ;
    m4[2][0] = m16[8]  ;
    m4[2][1] = m16[9]  ;
    m4[2][2] = m16[10] ;
    m4[2][3] = m16[11] ;
    m4[3][0] = m16[12] ;
    m4[3][1] = m16[13] ;
    m4[3][2] = m16[14] ;
    m4[3][3] = m16[15] ;
}


Uint8 gfc_matrix16_invert(float m[16], float invOut[16])
{
    float inv[16], det;
    int i;
    
    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
    return true;
}

Uint8 gfc_matrix4_invert(GFC_Matrix4 mOut, GFC_Matrix4 mIn)
{
    float m[16];
    gfc_matrix4_to_matrix16(m,mIn);
    
    if (!gfc_matrix16_invert(m,m))return 0;
    gfc_matrix16_to_matrix4(mOut,m);
    return 1;
}


void gfc_matrix2_copy(GFC_Matrix2 d,GFC_Matrix2 s)
{
    if ((!d)||(!s))return;
    if (d == s)return;
    memcpy(d,s,sizeof(GFC_Matrix2));
}

void gfc_matrix3_copy(GFC_Matrix3 d,GFC_Matrix3 s)
{
    if ((!d)||(!s))return;
    if (d == s)return;
    memcpy(d,s,sizeof(GFC_Matrix3));
}

void gfc_matrix4_copy(GFC_Matrix4 d,GFC_Matrix4 s)
{
    if ((!d)||(!s))return;
    if (d == s)return;
    memcpy(d,s,sizeof(GFC_Matrix4));
}


void gfc_matrix4_multiply_scalar(GFC_Matrix4 out,GFC_Matrix4 m1,float s)
{
    GFC_Matrix4 temp;
    temp[0][0] = s*m1[0][0];
    temp[0][1] = s*m1[0][1];
    temp[0][2] = s*m1[0][2];
    temp[0][3] = s*m1[0][3];

    temp[1][0] = s*m1[1][0];
    temp[1][1] = s*m1[1][1];
    temp[1][2] = s*m1[1][2];
    temp[1][3] = s*m1[1][3];

    temp[2][0] = s*m1[2][0];
    temp[2][1] = s*m1[2][1];
    temp[2][2] = s*m1[2][2];
    temp[2][3] = s*m1[2][3];

    temp[3][0] = s*m1[3][0];
    temp[3][1] = s*m1[3][1];
    temp[3][2] = s*m1[3][2];
    temp[3][3] = s*m1[3][3];
    gfc_matrix4_copy(out,temp);
}

void gfc_matrix3_multiply_scalar(GFC_Matrix3 out,GFC_Matrix3 m1,float s)
{
    GFC_Matrix3 temp;
    temp[0][0] = s*m1[0][0];
    temp[0][1] = s*m1[0][1];
    temp[0][2] = s*m1[0][2];

    temp[1][0] = s*m1[1][0];
    temp[1][1] = s*m1[1][1];
    temp[1][2] = s*m1[1][2];

    temp[2][0] = s*m1[2][0];
    temp[2][1] = s*m1[2][1];
    temp[2][2] = s*m1[2][2];
    gfc_matrix3_copy(out,temp);
}

void gfc_matrix2_multiply_scalar(GFC_Matrix2 out,GFC_Matrix2 m1,float s)
{
    GFC_Matrix2 temp;
    temp[0][0] = s*m1[0][0];
    temp[0][1] = s*m1[0][1];

    temp[1][0] = s*m1[1][0];
    temp[1][1] = s*m1[1][1];
    gfc_matrix2_copy(out,temp);
}

void gfc_matrix4_multiply(
    GFC_Matrix4 out,
    GFC_Matrix4 m2,
    GFC_Matrix4 m1)
{
    GFC_Matrix4 out1;//just in case its the same matrix
    out1[0][0] = m2[0][0]*m1[0][0] + m2[0][1]*m1[1][0] + m2[0][2]*m1[2][0] + m2[0][3]*m1[3][0];
    out1[0][1] = m2[0][0]*m1[0][1] + m2[0][1]*m1[1][1] + m2[0][2]*m1[2][1] + m2[0][3]*m1[3][1];
    out1[0][2] = m2[0][0]*m1[0][2] + m2[0][1]*m1[1][2] + m2[0][2]*m1[2][2] + m2[0][3]*m1[3][2];
    out1[0][3] = m2[0][0]*m1[0][3] + m2[0][1]*m1[1][3] + m2[0][2]*m1[2][3] + m2[0][3]*m1[3][3];

    out1[1][0] = m2[1][0]*m1[0][0] + m2[1][1]*m1[1][0] + m2[1][2]*m1[2][0] + m2[1][3]*m1[3][0];
    out1[1][1] = m2[1][0]*m1[0][1] + m2[1][1]*m1[1][1] + m2[1][2]*m1[2][1] + m2[1][3]*m1[3][1];
    out1[1][2] = m2[1][0]*m1[0][2] + m2[1][1]*m1[1][2] + m2[1][2]*m1[2][2] + m2[1][3]*m1[3][2];
    out1[1][3] = m2[1][0]*m1[0][3] + m2[1][1]*m1[1][3] + m2[1][2]*m1[2][3] + m2[1][3]*m1[3][3];
        
    out1[2][0] = m2[2][0]*m1[0][0] + m2[2][1]*m1[1][0] + m2[2][2]*m1[2][0] + m2[2][3]*m1[3][0];
    out1[2][1] = m2[2][0]*m1[0][1] + m2[2][1]*m1[1][1] + m2[2][2]*m1[2][1] + m2[2][3]*m1[3][1];
    out1[2][2] = m2[2][0]*m1[0][2] + m2[2][1]*m1[1][2] + m2[2][2]*m1[2][2] + m2[2][3]*m1[3][2];
    out1[2][3] = m2[2][0]*m1[0][3] + m2[2][1]*m1[1][3] + m2[2][2]*m1[2][3] + m2[2][3]*m1[3][3];
        
    out1[3][0] = m2[3][0]*m1[0][0] + m2[3][1]*m1[1][0] + m2[3][2]*m1[2][0] + m2[3][3]*m1[3][0];
    out1[3][1] = m2[3][0]*m1[0][1] + m2[3][1]*m1[1][1] + m2[3][2]*m1[2][1] + m2[3][3]*m1[3][1];
    out1[3][2] = m2[3][0]*m1[0][2] + m2[3][1]*m1[1][2] + m2[3][2]*m1[2][2] + m2[3][3]*m1[3][2];
    out1[3][3] = m2[3][0]*m1[0][3] + m2[3][1]*m1[1][3] + m2[3][2]*m1[2][3] + m2[3][3]*m1[3][3];
    gfc_matrix4_copy(out,out1);
}

void gfc_matrix3_multiply(
    GFC_Matrix3 out,
    GFC_Matrix3 m2,
    GFC_Matrix3 m1)
{
    GFC_Matrix3 out1;
    out1[0][0] = m2[0][0]*m1[0][0] + m2[0][1]*m1[1][0] + m2[0][2]*m1[2][0];
    out1[0][1] = m2[0][0]*m1[0][1] + m2[0][1]*m1[1][1] + m2[0][2]*m1[2][1];
    out1[0][2] = m2[0][0]*m1[0][2] + m2[0][1]*m1[1][2] + m2[0][2]*m1[2][2];

    out1[1][0] = m2[1][0]*m1[0][0] + m2[1][1]*m1[1][0] + m2[1][2]*m1[2][0];
    out1[1][1] = m2[1][0]*m1[0][1] + m2[1][1]*m1[1][1] + m2[1][2]*m1[2][1];
    out1[1][2] = m2[1][0]*m1[0][2] + m2[1][1]*m1[1][2] + m2[1][2]*m1[2][2];
        
    out1[2][0] = m2[2][0]*m1[0][0] + m2[2][1]*m1[1][0] + m2[2][2]*m1[2][0];
    out1[2][1] = m2[2][0]*m1[0][1] + m2[2][1]*m1[1][1] + m2[2][2]*m1[2][1];
    out1[2][2] = m2[2][0]*m1[0][2] + m2[2][1]*m1[1][2] + m2[2][2]*m1[2][2];
    gfc_matrix3_copy(out,out1);
}

void gfc_matrix2_multiply(
    GFC_Matrix2 out,
    GFC_Matrix2 m2,
    GFC_Matrix2 m1)
{
    GFC_Matrix2 out1;
    out1[0][0] = m2[0][0]*m1[0][0] + m2[0][1]*m1[1][0];
    out1[0][1] = m2[0][0]*m1[0][1] + m2[0][1]*m1[1][1];

    out1[1][0] = m2[1][0]*m1[0][0] + m2[1][1]*m1[1][0];
    out1[1][1] = m2[1][0]*m1[0][1] + m2[1][1]*m1[1][1];
    gfc_matrix2_copy(out,out1);
}


void gfc_matrix4_v_multiply(GFC_Vector4D *out,GFC_Vector4D vec,GFC_Matrix4 mat)
{
  float ox,oy,oz,ow;
  if (!out)return;
  ox=vec.x*mat[0][0] + vec.y*mat[1][0] + mat[2][0]*vec.z + mat[3][0]*vec.w;
  oy=vec.x*mat[0][1] + vec.y*mat[1][1] + mat[2][1]*vec.z + mat[3][1]*vec.w;
  oz=vec.x*mat[0][2] + vec.y*mat[1][2] + mat[2][2]*vec.z + mat[3][2]*vec.w;
  ow=vec.x*mat[0][3] + vec.y*mat[1][3] + mat[2][3]*vec.z + mat[3][3]*vec.w;
  out->x = ox;
  out->y = oy;
  out->z = oz;
  out->w = ow;
}

void gfc_matrix3_v_multiply(GFC_Vector3D *out,GFC_Vector3D vec,GFC_Matrix3 mat)
{
  float ox,oy,oz;
  if (!out)return;
  ox=vec.x*mat[0][0] + vec.y*mat[1][0] + mat[2][0]*vec.z;
  oy=vec.x*mat[0][1] + vec.y*mat[1][1] + mat[2][1]*vec.z;
  oz=vec.x*mat[0][2] + vec.y*mat[1][2] + mat[2][2]*vec.z;
  out->x = ox;
  out->y = oy;
  out->z = oz;
}

void gfc_matrix2_v_multiply(GFC_Vector2D *out,GFC_Vector2D vec,GFC_Matrix2 mat)
{
  float ox,oy;
  if (!out)return;
  ox=vec.x*mat[0][0] + vec.y*mat[1][0];
  oy=vec.x*mat[0][1] + vec.y*mat[1][1];
  
  out->x = ox;
  out->y = oy;
}

void gfc_matrix4_multiply_v(GFC_Vector4D * out,GFC_Matrix4 mat,GFC_Vector4D vec)
{
  float ox,oy,oz,ow;
  if (!out)return;
  ox=vec.x*mat[0][0] + vec.y*mat[0][1] + mat[0][2]*vec.z + mat[0][3]*vec.w;
  oy=vec.x*mat[1][0] + vec.y*mat[1][1] + mat[1][2]*vec.z + mat[1][3]*vec.w;
  oz=vec.x*mat[2][0] + vec.y*mat[2][1] + mat[2][2]*vec.z + mat[2][3]*vec.w;
  ow=vec.x*mat[3][0] + vec.y*mat[3][1] + mat[3][2]*vec.z + mat[3][3]*vec.w;
  out->x = ox;
  out->y = oy;
  out->z = oz;
  out->w = ow;
}

void gfc_matrix3_multiply_v(GFC_Vector3D * out,GFC_Matrix3 mat,GFC_Vector3D vec)
{
  float ox,oy,oz;
  if (!out)return;
  ox=vec.x*mat[0][0] + vec.y*mat[0][1] + mat[0][2]*vec.z;
  oy=vec.x*mat[1][0] + vec.y*mat[1][1] + mat[1][2]*vec.z;
  oz=vec.x*mat[2][0] + vec.y*mat[2][1] + mat[2][2]*vec.z;
  out->x = ox;
  out->y = oy;
  out->z = oz;
}

void gfc_matrix2_multiply_v(GFC_Vector2D * out,GFC_Matrix2 mat,GFC_Vector2D vec)
{
  float ox,oy;
  if (!out)return;
  ox=vec.x*mat[0][0] + vec.y*mat[0][1];
  oy=vec.x*mat[1][0] + vec.y*mat[1][1];
  out->x = ox;
  out->y = oy;
}


void gfc_matrix4_zero(GFC_Matrix4 zero)
{
    memset(zero,0,sizeof(GFC_Matrix4));
}

void gfc_matrix3_zero(GFC_Matrix3 zero)
{
    memset(zero,0,sizeof(GFC_Matrix3));
}

void gfc_matrix2_zero(GFC_Matrix2 zero)
{
    memset(zero,0,sizeof(GFC_Matrix2));
}


void gfc_matrix4_identity(GFC_Matrix4 one)
{
    gfc_matrix4_zero(one);
    one[0][0] = 1;
    one[1][1] = 1;
    one[2][2] = 1;
    one[3][3] = 1;
}

void gfc_matrix3_identity(GFC_Matrix3 one)
{
    gfc_matrix3_zero(one);
    one[0][0] = 1;
    one[1][1] = 1;
    one[2][2] = 1;
}

void gfc_matrix2_identity(GFC_Matrix2 one)
{
    gfc_matrix2_zero(one);
    one[0][0] = 1;
    one[1][1] = 1;
}

void gfc_matrix4_rotate_by_vector(
    GFC_Matrix4     out,
    GFC_Matrix4     m,
    GFC_Vector3D    v
)
{
    gfc_matrix4_rotate_y(out,m,v.y);
    gfc_matrix4_rotate_x(out,out,v.x);
    gfc_matrix4_rotate_z(out,out,v.z);
}

void gfc_matrix4_rotate_x(
    GFC_Matrix4     out,
    GFC_Matrix4     m,
    float       theta
)
{
    GFC_Matrix4     Result;
    
    gfc_matrix4_identity(Result);
    Result[0][0] = cos(-theta);
    Result[0][2] = sin(-theta);
    Result[2][0] = -sin(-theta);
    Result[2][2] = cos(-theta);
    gfc_matrix4_multiply(out,m,Result);
}

void gfc_matrix4_rotate_y(
    GFC_Matrix4     out,
    GFC_Matrix4     m,
    float       theta
)
{
    GFC_Matrix4     Result;
    
    gfc_matrix4_identity(Result);
    Result[1][1] = cos(-theta);
    Result[1][2] = -sin(-theta);
    Result[2][1] = sin(-theta);
    Result[2][2] = cos(-theta);
    
    gfc_matrix4_multiply(out,m,Result);
}

void gfc_matrix4_rotate_z(
    GFC_Matrix4     out,
    GFC_Matrix4     m,
    float       theta
)
{
    GFC_Matrix4     Result;
    
    gfc_matrix4_identity(Result);
    Result[0][0] = cos(-theta);
    Result[0][1] = -sin(-theta);
    Result[1][0] = sin(-theta);
    Result[1][1] = cos(-theta);
    gfc_matrix4_multiply(out,m,Result);
}


void gfc_matrix4_rotate(
    GFC_Matrix4     out,
    GFC_Matrix4     m,
    float           radians,
    GFC_Vector3D    axis
)
{
    gfc_matrix4_from_quaternion(out,gfc_vector4d(axis.x,axis.y,axis.z,radians));
    gfc_matrix4_multiply(out,m,out);
}

void gfc_matrix4_perspective(
    GFC_Matrix4     out,
    float      fov,
    float      aspect,
    float      near,
    float      far
)
{
    float halftanfov = tan(fov * 0.5);
    gfc_matrix4_zero(out);

    if (aspect == 0)
    {
        slog("gfc_matrix4_perspective: aspect ratio cannot be zero");
        return;
    }
    if (halftanfov == 0)
    {
        slog("gfc_matrix4_perspective: bad fov");
        return;
    }
    if (near == far)
    {
        slog("gfc_matrix4_perspective: near plane and far plane cannot be the same");
        return;
    }

    gfc_matrix4_zero(out);
    out[0][0] = 1 / (aspect * halftanfov);
    out[1][1] = 1 / (halftanfov);
    out[2][2] = - ((far + near) / (far - near));
    out[2][3] = -1;
    if ((far - near) == 0)
    {
        out[3][2] = 0;
    }
    else
    out[3][2] = -(2 * far * near) / (far - near);
    return;
}

void gfc_matrix4_view(
    GFC_Matrix4  out,
    GFC_Vector3D position,
    GFC_Vector3D target,
    GFC_Vector3D up
)
{
    GFC_Vector3D f,r,u;
    gfc_vector3d_sub(f,position,target);
    gfc_vector3d_normalize(&f);
    
    gfc_vector3d_cross_product(&r,up,f);
    gfc_vector3d_normalize(&r);
    
    gfc_vector3d_cross_product(&u,f,r);
    gfc_vector3d_normalize(&u);
 
    gfc_matrix4_identity(out);
    out[0][0] = r.x;
    out[1][0] = r.y;
    out[2][0] = r.z;
    out[0][1] = u.x;
    out[1][1] = u.y;
    out[2][1] = u.z;
    out[0][2] = f.x;
    out[1][2] = f.y;
    out[2][2] = f.z;
    out[3][0] = -gfc_vector3d_dot_product(r, position);
    out[3][1] = -gfc_vector3d_dot_product(u, position);
    out[3][2] = -gfc_vector3d_dot_product(f, position);
}

void gfc_matrix4_scale(GFC_Matrix4 out,GFC_Matrix4 in,GFC_Vector3D scale)
{
    GFC_Matrix4 m;
    gfc_matrix4_zero(m);
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
    m[3][3] = 1;
    
    gfc_matrix4_multiply(
        out,
        in,
        m
    );
}

void gfc_matrix4_make_translation(
    GFC_Matrix4 out,
    GFC_Vector3D move
)
{
    gfc_matrix4_identity(out);
    out[3][0] = move.x;
    out[3][1] = move.y;
    out[3][2] = move.z;
}

void gfc_matrix3_make_translation(
    GFC_Matrix3 out,
    GFC_Vector2D move
)
{
    gfc_matrix3_identity(out);
    out[2][0] = move.x;
    out[2][1] = move.y;
}

void gfc_matrix4_translate(GFC_Matrix4 out,GFC_Matrix4 in, GFC_Vector3D move)
{
    GFC_Matrix4 translate;
    gfc_matrix4_make_translation(translate,move);
    gfc_matrix4_multiply(out,in,translate);
}

void gfc_matrix3_translate(GFC_Matrix3 out,GFC_Matrix3 in, GFC_Vector2D move)
{
    GFC_Matrix3 translate;
    gfc_matrix3_make_translation(translate,move);
    gfc_matrix3_multiply(out,in,translate);
}


void gfc_matrix4_from_vectors_q(
    GFC_Matrix4 out,
    GFC_Vector3D translation,
    GFC_Vector4D quaternion,
    GFC_Vector3D scale
)
{
    GFC_Matrix4 rotateM;
        
    //M = T * R * S
    
    gfc_matrix4_identity(out);
    gfc_matrix4_scale(out,out,scale);

    gfc_matrix4_from_quaternion(rotateM,quaternion);
    gfc_matrix4_multiply(
            out,
            out,
            rotateM
        );
    
    gfc_matrix4_translate(out,out,translation);
}

void gfc_matrix4_from_quaternion(
    GFC_Matrix4     out,
    GFC_Vector4D    q)
{
    out[0][0] = 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z;
    out[0][1] = 2.0f*q.x*q.y + 2.0f*q.z*q.w;
    out[0][2] = 2.0f*q.x*q.z - 2.0f*q.y*q.w;
    out[0][3] = 0.0f;

    out[1][0] = 2.0f*q.x*q.y - 2.0f*q.z*q.w;
    out[1][1] = 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z;
    out[1][2] = 2.0f*q.z*q.y + 2.0f*q.x*q.w;
    out[1][3] = 0.0f;

    out[2][0]= 2.0f*q.x*q.z + 2.0f*q.y*q.w;
    out[2][1]= 2.0f*q.z*q.y - 2.0f*q.x*q.w;
    out[2][2] = 1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y;
    out[2][3] = 0.0f;

    out[3][0] = 0;
    out[3][1] = 0;
    out[3][2] = 0;
    out[3][3] = 1.f;
}

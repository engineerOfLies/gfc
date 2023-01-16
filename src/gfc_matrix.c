#include <stdio.h>
#include <string.h>
#include <math.h>

#include "gfc_matrix.h"
#include "simple_logger.h"

/*
 * Code has been adapted from glm to C for this project
 */
Vector3D gfc_unproject(Vector3D in,Matrix4 view, Matrix4 proj,Vector2D viewport)
{
    Vector3D out = {0,0,0};
    Matrix4 InvProj = {0},InvView = {0};
    Vector4D tmp,obj,eye;
    
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
        
    gfc_matrix_M_multiply_v(
        &eye,
        InvView,
        tmp);

    gfc_matrix_M_multiply_v(
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

void gfc_matrix4_slog(Matrix4 mat)
{
    slog("%f,%f,%f,%f",mat[0][0],mat[0][1],mat[0][2],mat[0][3]);
    slog("%f,%f,%f,%f",mat[1][0],mat[1][1],mat[1][2],mat[1][3]);
    slog("%f,%f,%f,%f",mat[2][0],mat[2][1],mat[2][2],mat[2][3]);
    slog("%f,%f,%f,%f",mat[3][0],mat[3][1],mat[3][2],mat[3][3]);
}

void gfc_matrix3_slog(Matrix3 mat)
{
    slog("%f,%f,%f",mat[0][0],mat[0][1],mat[0][2]);
    slog("%f,%f,%f",mat[1][0],mat[1][1],mat[1][2]);
    slog("%f,%f,%f",mat[2][0],mat[2][1],mat[2][2]);
}

void gfc_matrix2_slog(Matrix2 mat)
{
    slog("%f,%f",mat[0][0],mat[0][1]);
    slog("%f,%f",mat[1][0],mat[1][1]);
}

void gfc_matrix4_to_vectors(
    Matrix4 in,
    Vector3D *translation,
    Vector3D *rotation,
    Vector3D *scale)
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
    Matrix4 out,
    Vector3D translation,
    Vector3D rotation,
    Vector3D scale)
{
    gfc_matrix_identity(out);
    
    gfc_matrix_scale(out,scale);
    gfc_matrix_rotate_by_vector(out,out,rotation);
    gfc_matrix_translate(out,translation);
}

void gfc_matrix4_to_matrix16(float m16[16],Matrix4 m4)
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

void gfc_matrix16_to_matrix4(Matrix4 m4,float m16[16])
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

Uint8 gfc_matrix4_invert(Matrix4 mOut, Matrix4 mIn)
{
    float m[16];
    gfc_matrix4_to_matrix16(m,mIn);
    
    if (!gfc_matrix16_invert(m,m))return 0;
    gfc_matrix16_to_matrix4(mOut,m);
    return 1;
}


void gfc_matrix_copy(
    Matrix4 d,
    Matrix4 s
  )
{
    if ((!d)||(!s))return;
    if (d == s)return;
    memcpy(d,s,sizeof(Matrix4));
}


void gfc_matrix_multiply_scalar(Matrix4 out,Matrix4 m1,float s)
{
    Matrix4 temp;
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
  gfc_matrix_copy(out,temp);
}


void gfc_matrix_multiply(
    Matrix4 out,
    Matrix4 m2,
    Matrix4 m1
  )
{
  Matrix4 out1;
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
  gfc_matrix_copy(out,out1);
}

void gfc_matrix_v_multiply_M(
  Vector4D * out,
  Matrix4    mat,
  Vector4D   vec
)
{
  float x,y,z,w;
  float ox,oy,oz,ow;
  if (!out)return;
  x=vec.x;
  y=vec.y;
  z=vec.z;
  w=vec.w;
  ox=x*mat[0][0] + y*mat[1][0] + mat[2][0]*z + mat[3][0]*w;
  oy=x*mat[0][1] + y*mat[1][1] + mat[2][1]*z + mat[3][1]*w;
  oz=x*mat[0][2] + y*mat[1][2] + mat[2][2]*z + mat[3][2]*w;
  ow=x*mat[0][3] + y*mat[1][3] + mat[2][3]*z + mat[3][3]*w;
  out->x = ox;
  out->y = oy;
  out->z = oz;
  out->w = ow;
}

void gfc_matrix_M_multiply_v(
  Vector4D * out,
  Matrix4    mat,
  Vector4D   vec
)
{
  float x,y,z,w;
  float ox,oy,oz,ow;
  if (!out)return;
  x=vec.x;
  y=vec.y;
  z=vec.z;
  w=vec.w;
  ox=x*mat[0][0] + y*mat[0][1] + mat[0][2]*z + mat[0][3]*w;
  oy=x*mat[1][0] + y*mat[1][1] + mat[1][2]*z + mat[1][3]*w;
  oz=x*mat[2][0] + y*mat[2][1] + mat[2][2]*z + mat[2][3]*w;
  ow=x*mat[3][0] + y*mat[3][1] + mat[3][2]*z + mat[3][3]*w;
  out->x = ox;
  out->y = oy;
  out->z = oz;
  out->w = ow;
}

void gfc_matrix_zero(Matrix4 zero)
{
    memset(zero,0,sizeof(Matrix4));
}

void gfc_matrix_identity(Matrix4 one)
{
    gfc_matrix_zero(one);
    one[0][0] = 1;
    one[1][1] = 1;
    one[2][2] = 1;
    one[3][3] = 1;
}


void gfc_matrix_rotate_by_vector(
    Matrix4     out,
    Matrix4     m,
    Vector3D    v
)
{
    gfc_matrix_rotate_y(out,out,v.y);
    gfc_matrix_rotate_x(out,m,  v.x);
    gfc_matrix_rotate_z(out,out,v.z);
}

void gfc_matrix_rotate_x(
    Matrix4     out,
    Matrix4     m,
    float       theta
)
{
    Matrix4     Result;
    
    gfc_matrix_identity(Result);
    Result[0][0] = cos(-theta);
    Result[0][2] = sin(-theta);
    Result[2][0] = -sin(-theta);
    Result[2][2] = cos(-theta);
    gfc_matrix_multiply(out,m,Result);
}

void gfc_matrix_rotate_y(
    Matrix4     out,
    Matrix4     m,
    float       theta
)
{
    Matrix4     Result;
    
    gfc_matrix_identity(Result);
    Result[1][1] = cos(-theta);
    Result[1][2] = -sin(-theta);
    Result[2][1] = sin(-theta);
    Result[2][2] = cos(-theta);
    
    gfc_matrix_multiply(out,m,Result);
}

void gfc_matrix_rotate_z(
    Matrix4     out,
    Matrix4     m,
    float       theta
)
{
    Matrix4     Result;
    
    gfc_matrix_identity(Result);
    Result[0][0] = cos(-theta);
    Result[0][1] = -sin(-theta);
    Result[1][0] = sin(-theta);
    Result[1][1] = cos(-theta);
    gfc_matrix_multiply(out,m,Result);
}


void gfc_matrix_rotate(
    Matrix4     out,
    Matrix4     m,
    float       degree,
    Vector3D    axis
)
{
    Matrix4 Rotate;
    Matrix4 Result;
    Vector3D temp;
    float a = degree;
    float c = cos(a);
    float s = sin(a);

    vector3d_normalize(&axis);
    
    vector3d_scale(temp,axis,(1 - c));

    Rotate[0][0] = c + temp.x * axis.x;
    Rotate[0][1] = temp.x * axis.y + s * axis.z;
    Rotate[0][2] = temp.x * axis.z - s * axis.y;

    Rotate[1][0] = temp.y * axis.x - s * axis.z;
    Rotate[1][1] = c + temp.y * axis.y;
    Rotate[1][2] = temp.y * axis.z + s * axis.x;

    Rotate[2][0] = temp.z * axis.x + s * axis.y;
    Rotate[2][1] = temp.z * axis.y - s * axis.x;
    Rotate[2][2] = c + temp.z * axis.z;

    Result[0][0] = m[0][0] * Rotate[0][0] + m[1][0] * Rotate[0][1] + m[2][0] * Rotate[0][2];
    Result[0][1] = m[0][1] * Rotate[0][0] + m[1][1] * Rotate[0][1] + m[2][1] * Rotate[0][2];
    Result[0][2] = m[0][2] * Rotate[0][0] + m[1][2] * Rotate[0][1] + m[2][2] * Rotate[0][2];
    Result[0][3] = m[0][3] * Rotate[0][0] + m[1][3] * Rotate[0][1] + m[2][3] * Rotate[0][2];

    Result[1][0] = m[0][0] * Rotate[1][0] + m[1][0] * Rotate[1][1] + m[2][0] * Rotate[1][2];
    Result[1][1] = m[0][1] * Rotate[1][0] + m[1][1] * Rotate[1][1] + m[2][1] * Rotate[1][2];
    Result[1][2] = m[0][2] * Rotate[1][0] + m[1][2] * Rotate[1][1] + m[2][2] * Rotate[1][2];
    Result[1][3] = m[0][3] * Rotate[1][0] + m[1][3] * Rotate[1][1] + m[2][3] * Rotate[1][2];

    Result[2][0] = m[0][0] * Rotate[2][0] + m[1][0] * Rotate[2][1] + m[2][0] * Rotate[2][2];
    Result[2][1] = m[0][1] * Rotate[2][0] + m[1][1] * Rotate[2][1] + m[2][1] * Rotate[2][2];
    Result[2][2] = m[0][2] * Rotate[2][0] + m[1][2] * Rotate[2][1] + m[2][2] * Rotate[2][2];
    Result[2][3] = m[0][3] * Rotate[2][0] + m[1][3] * Rotate[2][1] + m[2][3] * Rotate[2][2];

    Result[3][0] = m[3][0];
    Result[3][1] = m[3][1];
    Result[3][2] = m[3][2];
    Result[3][3] = m[3][3];
    
    gfc_matrix_copy(out,Result);

}

void gfc_matrix_perspective(
    Matrix4     out,
    float      fov,
    float      aspect,
    float      near,
    float      far
)
{
    float halftanfov = tan(fov * 0.5);
    gfc_matrix_zero(out);

    if (aspect == 0)
    {
        slog("gfc_matrix_perspective: aspect ratio cannot be zero");
        return;
    }
    if (halftanfov == 0)
    {
        slog("gfc_matrix_perspective: bad fov");
        return;
    }
    if (near == far)
    {
        slog("gfc_matrix_perspective: near plane and far plane cannot be the same");
        return;
    }

    gfc_matrix_zero(out);
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

void gfc_matrix_view(
    Matrix4  out,
    Vector3D position,
    Vector3D target,
    Vector3D up
)
{
    Vector3D f,s,u;
    vector3d_sub(f,target,position);
    vector3d_normalize(&f);
    
    vector3d_cross_product(&s,f,up);
    vector3d_normalize(&s);
    
    vector3d_cross_product(&u,s,f);
 
    gfc_matrix_identity(out);
    out[0][0] = s.x;
    out[1][0] = s.y;
    out[2][0] = s.z;
    out[0][1] = u.x;
    out[1][1] = u.y;
    out[2][1] = u.z;
    out[0][2] = -f.x;
    out[1][2] = -f.y;
    out[2][2] = -f.z;
    out[3][0] = vector3d_dot_product(s, position)?-vector3d_dot_product(s, position):0;
    out[3][1] = vector3d_dot_product(u, position)?-vector3d_dot_product(u, position):0;
    out[3][2] = vector3d_dot_product(f, position)?vector3d_dot_product(f, position):0;
    
}

void gfc_matrix_scale(
    Matrix4 out,
    Vector3D scale
)
{
    Matrix4 m;
    gfc_matrix_zero(m);
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
    m[3][3] = 1;
    
    gfc_matrix_multiply(
        out,
        out,
        m
    );
}

void gfc_matrix_make_translation(
    Matrix4 out,
    Vector3D move
)
{
    if (!out)return;
    gfc_matrix_identity(out);
    out[3][0] = move.x;
    out[3][1] = move.y;
    out[3][2] = move.z;
}

void gfc_matrix_translate(
    Matrix4 out,
    Vector3D move
)
{
    Matrix4 translate,temp;
    gfc_matrix_make_translation(translate,move);
    gfc_matrix_multiply(temp,out,translate);
    gfc_matrix_copy(out,temp);
}

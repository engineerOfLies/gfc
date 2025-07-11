#ifndef __GFC_VECTOR_H__
#define __GFC_VECTOR_H__

/**
 * gfc gfc_vector
 * @license The MIT License (MIT)
   @copyright Copyright (c) 2019 EngineerOfLies
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

#include "gfc_types.h"

/*
 * The floating point GFC_Vector Types
 * Not to be confused with the gfc_vector lists from STL
 */
typedef struct
{
  float x;
  float y;
}GFC_Vector2D;

typedef struct
{
  float x;
  float y;
  float z;
}GFC_Vector3D;

typedef struct
{
  float x;
  float y;
  float z;
  float w;
}GFC_Vector4D;

/**
 * The integer space gfc_vector types:
 */
typedef struct
{
    int x,y;
}GFC_Vector2I;

typedef struct
{
    int x,y,z;
}GFC_Vector3I;

typedef struct
{
    int x,y,z,w;
}GFC_Vector4I;

//unsigned 8 bit gfc_vectors
typedef struct
{
    Uint8 x,y;
}GFC_Vector2UI8;

typedef struct
{
    Uint8 x,y,z;
}GFC_Vector3UI8;

typedef struct
{
    Uint8 x,y,z,w;
}GFC_Vector4UI8;

/**
 * @brief throw this in with the variable arguments in a printf to shorthand the slogging of a vector
 * It only shortens the work if you use auto-complete
 */
#define gfc_vector3d_to_slog(vec) vec.x,vec.y,vec.z

/**
 * @brief create and return an GFC_Vector2D
 */
GFC_Vector2D gfc_vector2d(float x, float y);
GFC_Vector3D gfc_vector3d(float x, float y, float z);
GFC_Vector4D gfc_vector4d(float x, float y, float z, float w);

/**
 * @brief convert a 3d gfc_vector into a 2d gfc_vector, dropping the 3rd value
 * @param v the 3d gfc_vector
 * @return the new 2d gfc_vector
 */
GFC_Vector2D gfc_vector3dxy(GFC_Vector3D v);

/**
 * @brief convert a 4d gfc_vector into a 2d gfc_vector, dropping the 3rd and 4th value
 * @param v the 4d gfc_vector
 * @return the new 2d gfc_vector
 */
GFC_Vector2D gfc_vector4dxy(GFC_Vector4D v);

/**
 * @brief convert a 4d gfc_vector into a 3d gfc_vector, dropping the 4th value
 * @param v the 4d gfc_vector
 * @return the new 3d gfc_vector
 */
GFC_Vector3D gfc_vector4dxyz(GFC_Vector4D v);

/**
 * @brief convert a 2d gfc_vector into a 3d gfc_vector
 * @param v the 2d gfc_vector
 * @param z the new z component
 * @return the new 3d gfc_vector
 */
GFC_Vector3D gfc_vector2dz(GFC_Vector2D v,float z);

/**
 * @brief convert a 3d gfc_vector into a 4d gfc_vector
 * @param v the 3d gfc_vector
 * @param w the new w component
 * @return the new 4d gfc_vector
 */
GFC_Vector4D gfc_vector3dw(GFC_Vector3D v,float w);

/**
 * @brief convert a 2d gfc_vector into a 4d gfc_vector
 * @param v the 2d gfc_vector
 * @param z the new z component
 * @param w the new w component
 * @return the new 4d gfc_vector
 */
GFC_Vector4D gfc_vector2dzw(GFC_Vector2D v,float z,float w);

/**
 * @brief sets the outvector to a unit gfc_vector pointing at the angle specified
 * @param out output.  GFC_Vector will be set to a unit gfc_vector rotated by radians
 * @param radians specify the angle of the gfc_vector to be set.
 */
void gfc_vector3d_set_angle_by_radians(GFC_Vector3D *out,float radians);
void gfc_vector2d_set_angle_by_radians(GFC_Vector2D *out,float radians);

/**
 * @brief get a unit gfc_vector rotated off of the vertical axis
 * @param angle in radians off of vertical axis
 * @return a unit gfc_vector
 */
GFC_Vector2D gfc_vector2d_from_angle(float angle);

/**
 * @brief returns the angle of a gfc_vector in 2D space.
 * @param x the x component of the gfc_vector
 * @param y the y component of the gfc_vector
 * @return the angle of the gfc_vector in radians from vertical.
 */
float gfc_vector_angle(float x,float y);

/**
 * @brief returns the angle of a gfc_vector in 2D space.
 * @param v the gfc_vector
 * @return the angle of the gfc_vector in radian. off of the y axis, not x
 */
float gfc_vector2d_angle(GFC_Vector2D v);

/**
 * @brief check if the current scale requires flipping of rotations
 * @param scale the scaling factor.
 * @return -1 if rotations need to be flipped, or 1 otherwise
 * @note: zero size gfc_vectors return 1
 */
int gfc_vector2d_scale_flip_rotation(GFC_Vector2D scale);

/**
 * @brief returns the angle between the two angles.
 * Takes into account wraping around 0 and 360
 * units are degrees
 * @Param a one of the angles in question
 * @Param b one of the angles in question
 * @return the angle, in degrees, between the two angles
 */
float gfc_angle_between_degrees(float a, float b);

/**
 * @brief returns the angle between the two angles.
 * Takes into account wraping around 0 and 2PI
 * units are radians
 * @Param a one of the angles in question
 * @Param b one of the angles in question
 * @return the angle, in degrees, between the two angles
 */
float gfc_angle_between_radians(float a, float b);

/**
 * @brief prevent an angle from going beyond the range of 0 to 2Pi
 * @param a the angle to clamp
 */
void gfc_angle_clamp_radians(float *a);

/**
 * @brief prevent an angle from going beyond the range of 0 - 360
 * @param a the angle to clamp
 */
void gfc_angle_clamp_degrees(float *a);

/**
 * 3D GFC_Vector Math
 * These macros handle most of the common operations for gfc_vector math.
 */

#define gfc_vector4d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z,ar[3] = vec.w)
#define gfc_vector3d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z)
#define gfc_vector2d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y)

/**
 * @brief calculates the cross product of the two input gfc_vectors
 * @param out the resultant cross product
 * @param v1 one of the components
 * @param v2 one of the components
 */
void gfc_vector3d_cross_product(GFC_Vector3D *out, GFC_Vector3D v1, GFC_Vector3D v2);

/**
 * @brief Macro to get the dot product from a gfc_vector.
 *
 * @param a GFC_Vector3D component of the dot product
 * @param b GFC_Vector3D component of the dot product
 *
 * @return the calculated dot product
 */
#define gfc_vector4d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w)
#define gfc_vector3d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z)
#define gfc_vector2d_dot_product(a,b)      (a.x*b.x+a.y*b.y)


/**
 * @brief multiply two gfc_vectors together (x *x, y*y, etc)
 * @param a component of the multiplication
 * @param b component of the multiplication
 * @return a gfc_vector multiplication product
 */
GFC_Vector2D gfc_vector2d_multiply(GFC_Vector2D a, GFC_Vector2D b);
GFC_Vector3D gfc_vector3d_multiply(GFC_Vector3D a, GFC_Vector3D b);
GFC_Vector4D gfc_vector4d_multiply(GFC_Vector4D a, GFC_Vector4D b);

/**
 * @brief copies the data from one gfc_vector into another
 *
 * @param dst the destination gfc_vector
 * @param src the source gfc_vector
 */
#define gfc_vector2d_copy(dst,src)  (dst.x = src.x,dst.y = src.y)
#define gfc_vector3d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z)
#define gfc_vector4d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z,dst.w=src.w)

/**
 * @brief macro to check if two gfc_vectors are exactly the same
 * @param a one gfc_vector to test
 * @param b one gfc_vector to test
 * @return true if and only if all parameters are the same
 */
#define gfc_vector2d_compare(a,b)   ((a.x==b.x)&&(a.y==b.y))
#define gfc_vector3d_compare(a,b)   ((a.x==b.x)&&(a.y==b.y)&&(a.z==b.z))
#define gfc_vector4d_compare(a,b)   ((a.x==b.x)&&(a.y==b.y)&&(a.z==b.z)&&(a.w==b.w))

/**
 * @brief Macro to subtract two gfc_vectors
 * varient ending in p takes a pointer to GFC_Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of gfc_vectors
 *
 * @param dst result GFC_Vector3D output
 * @param a GFC_Vector3D input
 * @param b GFC_Vector3D input
 */
#define gfc_vector2d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y)
#define gfc_vector3d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y,dst.z=a.z-b.z)
#define gfc_vector4d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y,dst.z=a.z-b.z,dst.w=a.w-b.w)

/**
 * @brief Macro to add two gfc_vectors
 * varient ending in p takes a pointer to GFC_Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of gfc_vectors
 *
 * @param dst result Vector output
 * @param a GFC_Vector input
 * @param b GFC_Vector input
 */
#define gfc_vector2d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y)
#define gfc_vector3d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y,dst.z = a.z+b.z)
#define gfc_vector4d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y,dst.z = a.z+b.z,dst.w = a.w+b.w)

/**
 * @brief function version of the subtract, returns the new gfc_vector
 * @param a GFC_Vector3D input
 * @param b GFC_Vector3D input
 * @return the resulting gfc_vector
 */
GFC_Vector3D gfc_vector3d_subbed(GFC_Vector3D a, GFC_Vector3D b);

/**
 * @brief function version of the add, returns the new gfc_vector
 * @param a GFC_Vector3D input
 * @param b GFC_Vector3D input
 * @return the resulting gfc_vector
 */
GFC_Vector3D gfc_vector3d_added(GFC_Vector3D a, GFC_Vector3D b);

/**
 * @brief Macro to scale a gfc_vector by a scalar value
 * varient ending in p takes a pointer to GFC_Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of gfc_vectors
 *
 * @param dst result GFC_Vector3D output
 * @param src GFC_Vector3D input
 * @Param factpr the scalar value to scale the gfc_vector by.
 */
#define gfc_vector2d_scale(dst,src,factor) (dst.x = src.x *(factor),\
                                         dst.y = src.y *(factor))
#define gfc_vector3d_scale(dst,src,factor) (dst.x = src.x *(factor),\
                                         dst.y = src.y *(factor),\
                                         dst.z = src.z *(factor))
#define gfc_vector4d_scale(dst,src,factor) (dst.x = src.x *(factor),\
                                         dst.y = src.y *(factor),\
                                         dst.z = src.z *(factor),\
                                         dst.w = src.w *(factor))

/**
 * @brief Macro to scale a gfc_vector by a gfc_vector of the same dimensions
 * varient ending in p takes a pointer to GFC_Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of gfc_vectors
 *
 * @param dst result GFC_Vector3D output
 * @param src GFC_Vector3D input
 * @Param factpr the scalar value to scale the gfc_vector by.
 */
#define gfc_vector2d_scale_by(dst,src,factor) (dst.x = src.x *(factor.x),\
                                         dst.y = src.y *(factor.y))

#define gfc_vector3d_scale_by(dst,src,factor) (dst.x = src.x *(factor.x),\
                                         dst.y = src.y *(factor.y),\
                                         dst.z = src.z *(factor.z))

#define gfc_vector4d_scale_by(dst,src,factor) (dst.x = src.x *(factor.x),\
                                         dst.y = src.y *(factor.y),\
                                         dst.z = src.z *(factor.z),\
                                         dst.w = src.w *(factor.w))

/**
 * @brief Macro that sets gfc_vector to zero.
 * @param a MglVect[2D|3D|4D] input
 */

#define gfc_vector2d_clear(a)       (a.x=0,a.y=0)
#define gfc_vector3d_clear(a)       (a.x=0,a.y=0,a.z=0)
#define gfc_vector4d_clear(a)       (a.x=0,a.y=0,a.z=0,a.w=0)

/**
 * @brief macro to test if a gfc_vector is zero
 * @param a the gfc_vector to test
 * @return true if it is zero, false otherwise
 */
#define gfc_vector2d_is_zero(a)     ((a.x==0)&&(a.y==0))
#define gfc_vector3d_is_zero(a)     ((a.x==0)&&(a.y==0)&&(a.z==0))
#define gfc_vector4d_is_zero(a)     ((a.x==0)&&(a.y==0)&&(a.z==0)&&(a.w==0))


/**
 * @brief Macro to get the negative of a gfc_vector
 *
 * @param src MglVect[2D|3D|4D] input
 * @param dst MglVect[2D|3D|4D] negated output
 */
#define gfc_vector2d_negate(dst,src)      (dst.x = -1 * src.x,dst.y = -1 * src.y)
#define gfc_vector3d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y,dst.z = -src.z)
#define gfc_vector4d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y,dst.z = -src.z,dst.w = -src.w)

/**
 * @brief Macro to set the components of the gfc_vector
 *
 * @param v GFC_Vector3D output
 * @param a float x component
 * @param b float y component
 * @param c float z component (only in 3D & 4D version)
 * @param d float w component (only in 4D version)
 */
#define gfc_vector2d_set(v, a, b)  (v.x=(a), v.y=(b))
#define gfc_vector3d_set(v, a, b, c)  (v.x=(a), v.y=(b), v.z=(c))
#define gfc_vector4d_set(v, a, b, c,d)  (v.x=(a), v.y=(b), v.z=(c), v.w=(d))

/**
 * @brief allocate an initialize to zero a vector
 * @return NULL on memory error, or a pointer to a vector that must be free()'d
 */
GFC_Vector2D *gfc_vector2d_new();
GFC_Vector3D *gfc_vector3d_new();
GFC_Vector4D *gfc_vector4d_new();

/**
 * @brief allocate and copy the data from a vector
 * @param old the vector to copy
 * @return NULL on memory error, or a pointer to a vector that must be free()'d
 */
GFC_Vector2D *gfc_vector2d_dup(GFC_Vector2D old);
GFC_Vector3D *gfc_vector3d_dup(GFC_Vector3D old);
GFC_Vector4D *gfc_vector4d_dup(GFC_Vector4D old);

/**
 * @brief reflect a gfc_vector about a normal.  Simulates "Bounce"
 * @param out the resultant gfc_vector
 * @param normal the normal gfc_vector for the surface that is providing the bounce
 * @param in the input gfc_vector that will bounce off of the normal
 */
void gfc_vector2d_reflect(GFC_Vector2D *out, GFC_Vector2D normal,GFC_Vector2D in);
void gfc_vector3d_reflect(GFC_Vector3D *out, GFC_Vector3D normal,GFC_Vector3D in);
void gfc_vector4d_reflect(GFC_Vector4D *out, GFC_Vector4D normal,GFC_Vector4D in);

/**
 * @brief normalizes the gfc_vector passed.  does nothing for a zero length gfc_vector.
 * @param v pointer to the gfc_vector to be normalized.
 * @return the magnitude of the vector provided
 */
float gfc_vector2d_magnitude(GFC_Vector2D V);
float gfc_vector3d_magnitude(GFC_Vector3D V);
float gfc_vector4d_magnitude(GFC_Vector4D V);

/**
 * @brief get the magnitude of the difference of the two gfc_vectors
 * @return the magnitude between the two vectors
 */
float gfc_vector2d_magnitude_between(GFC_Vector2D a,GFC_Vector2D b);
float gfc_vector3d_magnitude_between(GFC_Vector3D a,GFC_Vector3D b);
float gfc_vector4d_magnitude_between(GFC_Vector4D a,GFC_Vector4D b);

/**
 * @brief get the magnitude of the difference of the two gfc_vectors
 * @return the magnitude squared between the two vectors (avoids a square root)
 */
float gfc_vector2d_magnitude_between_squared(GFC_Vector2D a,GFC_Vector2D b);
float gfc_vector3d_magnitude_between_squared(GFC_Vector3D a,GFC_Vector3D b);
float gfc_vector4d_magnitude_between_squared(GFC_Vector4D a,GFC_Vector4D b);

/**
 * @brief normalize a gfc_vector
 * @param V input and output. this is changed by the process
 * @note changes the gfc_vector
 */
void gfc_vector2d_normalize (GFC_Vector2D *V);
void gfc_vector3d_normalize (GFC_Vector3D *V);
void gfc_vector4d_normalize (GFC_Vector4D *V);

/**
 * @brief get the normal of a gfc_vector
 * @note original gfc_vector is unchanged
 * @param v the input vector
 * @return a new unit length gfc_vector
 */
GFC_Vector2D gfc_vector2d_get_normal(GFC_Vector2D v);
GFC_Vector3D gfc_vector3d_get_normal(GFC_Vector3D v);
GFC_Vector4D gfc_vector4d_get_normal(GFC_Vector4D v);

/**
 * @brief returns the magnitude squared, which is faster than getting the magnitude
 * which would involve taking the square root of a floating point number.
 * @param V the gfc_vector to get the magnitude for
 * @return the square of the magnitude of V
 */
float gfc_vector2d_magnitude_squared(GFC_Vector2D V);
float gfc_vector3d_magnitude_squared(GFC_Vector3D V);
float gfc_vector4d_magnitude_squared(GFC_Vector4D V);

/**
 * @brief checks if the magnitude of V against size.  It does this without
 * doing square roots, which are costly.  It will still do floating point multiplication
 * @param V the gfc_vector to check
 * @param size the magnitude to check against
 * @return -1 f V is less than size, 0 if equal or 1 if size is greater than V
 */
int  gfc_vector2d_magnitude_compare(GFC_Vector2D V,float size);
int  gfc_vector3d_magnitude_compare(GFC_Vector3D V,float size);
int  gfc_vector4d_magnitude_compare(GFC_Vector4D V,float size);

/**
 * @brief scales the gfc_vector to the specified length without changing direction
 * No op is magnitude is 0 or V is NULL
 * @param V a pointer to the gfc_vector to scale
 * @param magnitude the new length for the gfc_vector
 */
void gfc_vector2d_set_magnitude(GFC_Vector2D * V,float magnitude);
void gfc_vector3d_set_magnitude(GFC_Vector3D * V,float magnitude);
void gfc_vector4d_set_magnitude(GFC_Vector4D * V,float magnitude);

/**
 * @brief checks if the distance between the two points provided is less than size.
 * @param p1 one point for the distance check
 * @param p2 another point for the distance check
 * @param size the value to check against
 * @return true if the distance between P1 and P2 is less than size, false otherwise
 */
Bool gfc_vector2d_distance_between_less_than(GFC_Vector2D p1,GFC_Vector2D p2,float size);
Bool gfc_vector3d_distance_between_less_than(GFC_Vector3D p1,GFC_Vector3D p2,float size);
Bool gfc_vector4d_distance_between_less_than(GFC_Vector4D p1,GFC_Vector4D p2,float size);

/**
 * @brief given a rotation, get the component gfc_vectors  (in radians!)
 * @param angles the input rotation for each axis
 * @note as of now, roll is ignored
 * @param forward output optional calculated forward gfc_vector
 * @param right output optional calculated right gfc_vector
 * @param up output optional calculated up gfc_vector
 * @note this works by getting the cross products relative to the world UP (or Right if the vector is straight up/down)
 */
void gfc_vector3d_angle_vectors(GFC_Vector3D angles, GFC_Vector3D *forward, GFC_Vector3D *right, GFC_Vector3D *up);

/**
 * @brief from a forward vector, get the right and up vectors for it
 * @note this assumes +z is the world up
 * @param forward the input direction
 * @param right [output] the vector perpindicular to foward to the right
 * @param up [output] the vector perpindicular to foward up
 */
void gfc_vector3d_get_directions(GFC_Vector3D forward, GFC_Vector3D *right, GFC_Vector3D *up);

/**
 * @brief given a rotation, get the component gfc_vectors  (in radians!)
 * @param angles the input rotation for each axis
 * @param forward output optional calculated forward gfc_vector
 * @param right output optional calculated right gfc_vector
 * @param up output optional calculated up gfc_vector
 * @note this will fail to gimble lock
 */
void gfc_vector3d_angle_vectors2(GFC_Vector3D angles, GFC_Vector3D *forward, GFC_Vector3D *right, GFC_Vector3D *up);

/**
 * @brief given a gfc_vector, get the angles, in radians for yaw,roll, pitch
 * @param gfc_vector the gfc_vector to assess
 * @param angles the angles in radians (x,y,z) as (yaw,roll,pitch)
 * @note roll will always be zero
 * @note adapted from the quake2 source 'vectoangles' function
 */
void gfc_vector3d_angles (GFC_Vector3D gfc_vector, GFC_Vector3D * angles);

/**
 * @brief rotate a 2D gfc_vector by the angle specified (in radians)
 * @param in the input gfc_vector
 * @param angle the angle to rotate by
 * @return the new gfc_vector
 */
GFC_Vector2D gfc_vector2d_rotate(GFC_Vector2D in, float angle);

/**
 * @brief rotate a point around a center point
 * @param point the point to rotate
 * @param angle the amount to rotate in radians
 * @param center the point around which to rotate
 * @return the new location for the point
 */
GFC_Vector2D gfc_vector2d_rotate_around_center(GFC_Vector2D point,float angle, GFC_Vector2D center);

/**
 * @brief rotate an GFC_Vector3D about another gfc_vector
 * @param dst output result
 * @param dir the gfc_vector to rotate about
 * @param point the point to rotate
 * @param degrees how far to rotate
 */
void gfc_vector3d_rotate_about_vector(GFC_Vector3D *dst, GFC_Vector3D dir, GFC_Vector3D point, float degrees);

/**
 * @brief rotate a vector around the axis based on the angle provided
 * @param vect the input and output if NULL this is a no-op
 * @param angle the amount to rotate in radians
 */
void gfc_vector3d_rotate_about_x(GFC_Vector3D *vect, float angle);
void gfc_vector3d_rotate_about_y(GFC_Vector3D *vect, float angle);
void gfc_vector3d_rotate_about_z(GFC_Vector3D *vect, float angle);

/**
 * @brief move a point towards another point a set distance
 * @note this is a common quality of life function
 * @param out the output gfc_vector, if it is NULL, this is a no-op
 * @param point the point to modify
 * @param destination the point to move towards
 * @param distance the amount to move the point
 */
void gfc_vector2d_move_towards(GFC_Vector2D *out, GFC_Vector2D point, GFC_Vector2D destination, float distance);

/**
 * @brief given a vector multiply its components by gfc_crandom() range of [-1.0 - 1.0]
 * @param out [output] results saved here
 * @param in the input vector.
 */
void gfc_vector3d_randomize(GFC_Vector3D *out,GFC_Vector3D in);

#endif

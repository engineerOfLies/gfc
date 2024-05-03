#ifndef __GFC_VECTOR_H__
#define __GFC_VECTOR_H__

/**
 * gfc vector
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
 * The floating point Vector Types
 * Not to be confused with the vector lists from STL
 */
typedef struct
{
  float x;
  float y;
}Vector2D;

typedef struct
{
  float x;
  float y;
  float z;
}Vector3D;

typedef struct
{
  float x;
  float y;
  float z;
  float w;
}Vector4D;

/**
 * The integer space vector types:
 */
typedef struct
{
    int x,y;
}Vector2I;

typedef struct
{
    int x,y,z;
}Vector3I;

typedef struct
{
    int x,y,z,w;
}Vector4I;

//unsiened 8 bit vectors
typedef struct
{
    Uint8 x,y;
}Vector2UI8;

typedef struct
{
    Uint8 x,y,z;
}Vector3UI8;

typedef struct
{
    Uint8 x,y,z,w;
}Vector4UI8;

#define vector_in_rect(v, r)  ((v.x >= r.x)&&(v.x < (r.x + r.w))&&(v.y >= r.y)&&(v.y < (r.y + r.h)))

/**
 * @brief create and return an Vector2D
 */
Vector2D vector2d(float x, float y);

/**
 * @brief create and return an Vector3D
 */
Vector3D vector3d(float x, float y, float z);

/**
 * @brief create and return an Vector4D
 */
Vector4D vector4d(float x, float y, float z, float w);

/**
 * @brief convert a 3d vector into a 2d vector, dropping the 3rd value
 * @param v the 3d vector
 * @return the new 2d vector
 */
Vector2D vector3dxy(Vector3D v);

/**
 * @brief convert a 4d vector into a 2d vector, dropping the 3rd and 4th value
 * @param v the 4d vector
 * @return the new 2d vector
 */
Vector2D vector4dxy(Vector4D v);

/**
 * @brief convert a 4d vector into a 3d vector, dropping the 4th value
 * @param v the 4d vector
 * @return the new 3d vector
 */
Vector3D vector4dxyz(Vector4D v);

/**
 * @brief convert a 2d vector into a 3d vector
 * @param v the 2d vector
 * @param z the new z component
 * @return the new 3d vector
 */
Vector3D vector2dxyz(Vector2D v,float z);

/**
 * @brief convert a 3d vector into a 4d vector
 * @param v the 3d vector
 * @param w the new w component
 * @return the new 4d vector
 */
Vector4D vector3dxyzw(Vector3D v,float w);

/**
 * @brief convert a 2d vector into a 4d vector
 * @param v the 2d vector
 * @param z the new z component
 * @param w the new w component
 * @return the new 4d vector
 */
Vector4D vector2dxyzw(Vector2D v,float z,float w);

/**
 * @brief sets the outvector to a unit vector pointing at the angle specified
 * @param out output.  Vector will be set to a unit vector rotated by radians
 * @param radians specify the angle of the vector to be set.
 */
void vector3d_set_angle_by_radians(Vector3D *out,float radians);

void vector2d_set_angle_by_radians(Vector2D *out,float radians);

/**
 * @brief get a unit vector rotated off of the vertical axis
 * @param angle in radians off of vertical axis
 * @return a unit vector
 */
Vector2D vector2d_from_angle(float angle);

/**
 * @brief returns the angle of a vector in 2D space.
 * @param x the x component of the vector
 * @param y the y component of the vector
 * @return the angle of the vector in degrees from vertical.
 */
float vector_angle(float x,float y);

/**
 * @brief returns the angle of a vector in 2D space.
 * @param v the vector
 * @return the angle of the vector in degrees.
 */
float vector2d_angle(Vector2D v);

/**
 * @brief check if the current scale requires flipping of rotations
 * @param scale the scaling factor.
 * @return -1 if rotations need to be flipped, or 1 otherwise
 * @note: zero size vectors return 1
 */
int vector2d_scale_flip_rotation(Vector2D scale);

/**
 * @brief returns the angle between the two angles.
 * Takes into account wraping around 0 and 360
 * units are degrees
 * @Param a one of the angles in question
 * @Param b one of the angles in question
 * @return the angle, in degrees, between the two angles
 */
float angle_between_degrees(float a, float b);

/**
 * @brief returns the angle between the two angles.
 * Takes into account wraping around 0 and 2PI
 * units are radians
 * @Param a one of the angles in question
 * @Param b one of the angles in question
 * @return the angle, in degrees, between the two angles
 */
float angle_between_radians(float a, float b);

/**
 * @brief prevent an angle from going beyond the range of +/- 2Pi
 * @param a the angle to clamp
 */
void angle_clamp_radians(float *a);

/**
 * @brief prevent an angle from going beyond the range of 0 - 360
 * @param a the angle to clamp
 */
void angle_clamp_degrees(float *a);

#define vector3d_to_slog(vec) vec.x,vec.y,vec.z

/**
 * 3D Vector Math
 * These macros handle most of the common operations for vector math.
 */

#define vector4d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z,ar[3] = vec.w)
#define vector3d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z)
#define vector2d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y)

/**
 * @brief calculates the cross product of the two input vectors
 * @param out the resultant cross product
 * @param v1 one of the components
 * @param v2 one of the components
 */
void vector3d_cross_product(Vector3D *out, Vector3D v1, Vector3D v2);

/**
 * @brief Macro to get the dot product from a vector.
 *
 * @param a Vector3D component of the dot product
 * @param b Vector3D component of the dot product
 *
 * @return the calculated dot product
 */
#define vector4d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w)
#define vector3d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z)
#define vector2d_dot_product(a,b)      (a.x*b.x+a.y*b.y)


/**
 * @brief multiply two vectors together (x *x, y*y, etc)
 * @param a component of the multiplication
 * @param b component of the multiplication
 * @return a vector multiplication product
 */
Vector2D vector2d_multiply(Vector2D a, Vector2D b);
Vector3D vector3d_multiply(Vector3D a, Vector3D b);
Vector4D vector4d_multiply(Vector4D a, Vector4D b);

/**
 * @brief copies the data from one vector into another
 *
 * @param dst the destination vector
 * @param src the source vector
 */
#define vector2d_copy(dst,src)  (dst.x = src.x,dst.y = src.y)
#define vector3d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z)
#define vector4d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z,dst.w=src.w)

/**
 * @brief checks if vectors are exactly matches of each other
 *
 * @param a one vector to check
 * @param b other vector to check
 */
#define vector2d_equal(a,b) ((a.x == b.x)&&(a.y == b.y))
#define vector3d_equal(a,b) ((a.x == b.x)&&(a.y == b.y)&&(a.z == b.z))
#define vector4d_equal(a,b) ((a.x == b.x)&&(a.y == b.y)&&(a.z == b.z)&&(a.w == b.w))

/**
 * @brief Macro to subtract two vectors
 * varient ending in p takes a pointer to Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result Vector3D output
 * @param a Vector3D input
 * @param b Vector3D input
 */
#define vector2d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y)
#define vector3d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y,dst.z=a.z-b.z)
#define vector4d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y,dst.z=a.z-b.z,dst.w=a.w-b.w)

/**
 * @brief Macro to add two vectors
 * varient ending in p takes a pointer to Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result Vector3D output
 * @param a Vector3D input
 * @param b Vector3D input
 */
#define vector2d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y)
#define vector3d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y,dst.z = a.z+b.z)
#define vector4d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y,dst.z = a.z+b.z,dst.w = a.w+b.w)

/**
 * @brief function version of the subtract, returns the new vector
 * @param a Vector3D input
 * @param b Vector3D input
 * @return the resulting vector
 */
Vector3D vector3d_subbed(Vector3D a, Vector3D b);

/**
 * @brief function version of the add, returns the new vector
 * @param a Vector3D input
 * @param b Vector3D input
 * @return the resulting vector
 */
Vector3D vector3d_added(Vector3D a, Vector3D b);

/**
 * @brief Macro to scale a vector by a scalar value
 * varient ending in p takes a pointer to Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result Vector3D output
 * @param src Vector3D input
 * @Param factpr the scalar value to scale the vector by.
 */
#define vector2d_scale(dst,src,factor) (dst.x = src.x *(factor),\
                                         dst.y = src.y *(factor))
#define vector3d_scale(dst,src,factor) (dst.x = src.x *(factor),\
                                         dst.y = src.y *(factor),\
                                         dst.z = src.z *(factor))
#define vector4d_scale(dst,src,factor) (dst.x = src.x *(factor),\
                                         dst.y = src.y *(factor),\
                                         dst.z = src.z *(factor),\
                                         dst.w = src.w *(factor))

/**
 * @brief Macro to scale a vector by a vector of the same dimensions
 * varient ending in p takes a pointer to Vector3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result Vector3D output
 * @param src Vector3D input
 * @Param factpr the scalar value to scale the vector by.
 */
#define vector2d_scale_by(dst,src,factor) (dst.x = src.x *(factor.x),\
                                         dst.y = src.y *(factor.y))

#define vector3d_scale_by(dst,src,factor) (dst.x = src.x *(factor.x),\
                                         dst.y = src.y *(factor.y),\
                                         dst.z = src.z *(factor.z))

#define vector4d_scale_by(dst,src,factor) (dst.x = src.x *(factor.x),\
                                         dst.y = src.y *(factor.y),\
                                         dst.z = src.z *(factor.z),\
                                         dst.w = src.w *(factor.w))

/**
 * @brief Macro that sets vector to zero.
 * @param a MglVect[2D|3D|4D] input
 */

#define vector2d_clear(a)       (a.x=0,a.y=0)
#define vector3d_clear(a)       (a.x=0,a.y=0,a.z=0)
#define vector4d_clear(a)       (a.x=0,a.y=0,a.z=0,a.w=0)

/**
 * @brief macro to test if a vector is zero
 * @param a the vector to test
 * @return true if it is zero, false otherwise
 */
#define vector2d_is_zero(a)     ((a.x==0)&&(a.y==0))
#define vector3d_is_zero(a)     ((a.x==0)&&(a.y==0)&&(a.z==0))
#define vector4d_is_zero(a)     ((a.x==0)&&(a.y==0)&&(a.z==0)&&(a.w==0))

/**
 * @brief macro to check if two vectors are exactly the same
 * @param a one vector to test
 * @param b one vector to test
 * @return true if and only if all parameters are the same
 */
#define vector2d_compare(a,b)   ((a.x==b.x)&&(a.y==b.y))
#define vector3d_compare(a,b)   ((a.x==b.x)&&(a.y==b.y)&&(a.z==b.z))
#define vector4d_compare(a,b)   ((a.x==b.x)&&(a.y==b.y)&&(a.z==b.z)&&(a.w==b.w))

/**
 * @brief Macro to get the negative of a vector
 *
 * @param src MglVect[2D|3D|4D] input
 * @param dst MglVect[2D|3D|4D] negated output
 */
#define vector2d_negate(dst,src)      (dst.x = -1 * src.x,dst.y = -1 * src.y)
#define vector3d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y,dst.z = -src.z)
#define vector4d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y,dst.z = -src.z,dst.w = -src.w)

/**
 * @brief Macro to set the components of the vector
 *
 * @param v Vector3D output
 * @param a float x component
 * @param b float y component
 * @param c float z component (only in 3D & 4D version)
 * @param d float w component (only in 4D version)
 */
#define vector2d_set(v, a, b)  (v.x=(a), v.y=(b))
#define vector3d_set(v, a, b, c)  (v.x=(a), v.y=(b), v.z=(c))
#define vector4d_set(v, a, b, c,d)  (v.x=(a), v.y=(b), v.z=(c), v.w=(d))

Vector2D *vector2d_new();
Vector3D *vector3d_new();
Vector4D *vector4d_new();

Vector2D *vector2d_dup(Vector2D old);
Vector3D *vector3d_dup(Vector3D old);
Vector4D *vector4d_dup(Vector4D old);

/**
 * @brief reflect a vector about a normal.  Simulates "Bounce"
 * @param out the resultant vector
 * @param normal the normal vector for the surface that is providing the bounce
 * @param in the input vector that will bounce off of the normal
 */
void vector2d_reflect(Vector2D *out, Vector2D normal,Vector2D in);
void vector3d_reflect(Vector3D *out, Vector3D normal,Vector3D in);
void vector4d_reflect(Vector4D *out, Vector4D normal,Vector4D in);


/**
 * @brief normalizes the vector passed.  does nothing for a zero length vector.
 *
 * @param v pointer to the vector to be normalized.
 */
float vector2d_magnitude(Vector2D V);
float vector3d_magnitude(Vector3D V);
float vector4d_magnitude(Vector4D V);

/**
 * @brief get the magnitude of the difference of the two vectors
 */
float vector2d_magnitude_between(Vector2D a,Vector2D b);
float vector3d_magnitude_between(Vector3D a,Vector3D b);
float vector4d_magnitude_between(Vector4D a,Vector4D b);

/**
 * @brief normalize a vector
 * @note changes the vector
 */
void vector2d_normalize (Vector2D *V);
void vector3d_normalize (Vector3D *V);
void vector4d_normalize (Vector4D *V);

/**
 * @brief get the normal of a vector
 * @note original vector is unchanged
 * @return a new unit vector
 */
Vector2D vector2d_get_normal(Vector2D v);
Vector3D vector3d_get_normal(Vector3D v);
Vector4D vector4d_get_normal(Vector4D v);

/**
 * @brief returns the magnitude squared, which is faster than getting the magnitude
 * which would involve taking the square root of a floating point number.
 * @param V the vector to get the magnitude for
 * @return the square of the magnitude of V
 */
float vector2d_magnitude_squared(Vector2D V);
float vector3d_magnitude_squared(Vector3D V);
float vector4d_magnitude_squared(Vector4D V);

/**
 * @brief checks if the magnitude of V against size.  It does this without
 * doing square roots, which are costly.  It will still do floating point multiplication
 * @param V the vector to check
 * @param size the magnitude to check against
 * @return -1 f V is less than size, 0 if equal or 1 if size is greater than V
 */
int  vector2d_magnitude_compare(Vector2D V,float size);
int  vector3d_magnitude_compare(Vector3D V,float size);
int  vector4d_magnitude_compare(Vector4D V,float size);

/**
 * @brief scales the vector to the specified length without changing direction
 * No op is magnitude is 0 or V is NULL
 * @param V a pointer to the vector to scale
 * @param magnitude the new length for the vector
 */
void vector2d_set_magnitude(Vector2D * V,float magnitude);
void vector3d_set_magnitude(Vector3D * V,float magnitude);
void vector4d_set_magnitude(Vector4D * V,float magnitude);

/**
 * @brief checks if the distance between the two points provided is less than size.
 * @param p1 one point for the distance check
 * @param p2 another point for the distance check
 * @param size the value to check against
 * @return true if the distance between P1 and P2 is less than size, false otherwise
 */
Bool vector2d_distance_between_less_than(Vector2D p1,Vector2D p2,float size);
Bool vector3d_distance_between_less_than(Vector3D p1,Vector3D p2,float size);
Bool vector4d_distance_between_less_than(Vector4D p1,Vector4D p2,float size);

/**
 * @brief given a rotation, get the component vectors  (in degrees!)
 * @param angles the input rotation
 * @param forward output optional calculated forward vector
 * @param right output optional calculated right vector
 * @param up output optional calculated up vector
 */
void vector3d_angle_vectors(Vector3D angles, Vector3D *forward, Vector3D *right, Vector3D *up);

/**
 * @brief given a vector, get the angles, in radians for yaw,roll, pitch
 * @param vector the vector to assess
 * @param angles the angles in radians (x,y,z) as (yaw,roll,pitch)
 * @note roll will always be zero
 * @note adapted from the quake2 source 'vectoangles' function
 */
void vector3d_angles (Vector3D vector, Vector3D * angles);

/**
 * @brief rotate a 2D vector by the angle specified (in radians)
 * @param in the input vector
 * @param angle the angle to rotate by
 * @return the new vector
 */
Vector2D vector2d_rotate(Vector2D in, float angle);

/**
 * @brief rotate a point around a center point
 * @param point the point to rotate
 * @param angle the amount to rotate in radians
 * @param center the point around which to rotate
 * @return the new location for the point
 */
Vector2D vector2d_rotate_around_center(Vector2D point,float angle, Vector2D center);

/**
 * @brief rotate an Vector3D about another vector
 * @param dst output result
 * @param dir the vector to rotate about
 * @param point the point to rotate
 * @param degrees how far to rotate
 */
void vector3d_rotate_about_vector(Vector3D *dst, Vector3D dir, Vector3D point, float degrees);

void vector3d_rotate_about_x(Vector3D *vect, float angle);
void vector3d_rotate_about_y(Vector3D *vect, float angle);
void vector3d_rotate_about_z(Vector3D *vect, float angle);

/**
 * @brief move a point towards another point a set distance
 * @note this is a common quality of life function
 * @param out the output vector, if it is NULL, this is a no-op
 * @param point the point to modify
 * @param destination the point to move towards
 * @param distance the amount to move the point
 */
void vector2d_move_towards(Vector2D *out, Vector2D point, Vector2D destination, float distance);

#endif

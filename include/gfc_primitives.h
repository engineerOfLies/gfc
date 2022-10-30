#ifndef __GFC_PRIMITIVES_H__
#define __GFC_PRIMITIVES_H__

#include "simple_json.h"

#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_color.h"


/**
 * @purpose gfc_primitives is meant to provide a common way to represent simple 3D shapes and test collisions based on them
 */

typedef struct
{
    Vector3D a,b;
}Edge3D;

typedef struct
{
    Vector3D a,b,c;
}Triangle3D;

typedef struct
{
    float x,y,z;   //vector normal of the plane
    float d;       //distance from origin
}Plane3D;

typedef struct
{
    float x,y,z,r; // position in space, and radius
}Sphere;

typedef struct
{
    float x,y,z;   //position of corner
    float w,h,d;   // width, height, and depth offsets
}Box;

typedef enum
{
    VT_POINT,
    VT_SPHERE,
    VT_EDGE,
    VT_PLANE,
    VT_TRIANLGE,
    VT_BOX
}PrimitiveTypes;

typedef struct
{
    PrimitiveTypes type;
    union
    {
        Vector3D p;//point
        Sphere s;
        Edge3D e;
        Plane3D pl;
        Triangle3D t;
        Box b;
    }s;
}Primitive;

/**
 * @brief make an axis aligned box based on its component
 * @param x the min x value
 * @param y the min y value
 * @param z the min z value
 * @param w the size along x
 * @param h the size along y
 * @param d the size along z
 * @return a set box
 */
Box gfc_box(float x, float y, float z, float w, float h, float d);

/**
 * @brief make a sphere based on its component
 * @param x the center x value
 * @param y the center y value
 * @param z the center z value
 * @param r the radius
 * @return a set sphere
 */
Sphere gfc_sphere(float x, float y, float z, float r);

/**
 * @brief make a plane based on its component
 * @param x the normal x value
 * @param y the normal y value
 * @param z the normal z value
 * @param d the distance from origin
 * @return a set sphere
 */
Plane3D gfc_plane3d(float x, float y, float z, float d);

/**
 * @brief make a triangle based on 3 points
 * @param a a point
 * @param b a point
 * @param c a point
 * @return a set triangle
 */
Triangle3D gfc_triangle(Vector3D a,Vector3D b,Vector3D c);

/**
 * @brief make a 3d edge from its 2 points
 * @param ax point a x position
 * @param ay point a y position
 * @param az point a z position
 * @param bx point b x position
 * @param by point b y position
 * @param bz point b z position
 * @return a set edge3d
 */
Edge3D gfc_edge3d(float ax,float ay,float az,float bx,float by,float bz);

/**
 * @brief make an edge from component vectors
 * @param a point a's position
 * @param b point b's potition
 * @return a set edge3d
 */
Edge3D gfc_edge3d_from_vectors(Vector3D a,Vector3D b);

//tests
/**
 * @brief check if a point is within a bounding box
 * @param p the point to check
 * @param b the bounding box to check
 * @return 1 if the point is inside, 0 if not
 */
Uint8 gfc_point_in_box(Vector3D p,Box b);

/**
 * @brief check if two boxes overlap
 * @param a one of the boxes
 * @param b the other box
 * @return 1 if there is any overlap, 0 if not
 */
Uint8 gfc_box_overlap(Box a,Box b);

/**
 * @brief check if a point is within a sphere
 * @param p the point to check
 * @param s the sphere the check
 * @return 1 if it is inside, 0 if not
 */
Uint8 gfc_point_in_sphere(Vector3D p,Sphere s);

/**
 * @brief check if two spheres overlap
 * @param A one sphere
 * @param B the other sphere
 * @return 1 if there is any overlap, 0 if not
 */
Uint8 gfc_sphere_overlap(Sphere A, Sphere B);

/**
 * @brief given a triangle, get its plane
 * @param t the triangle in question
 * @return the plane determined by the triangle
 */
Plane3D gfc_triangle_get_plane(Triangle3D t);

/**
 * @brief get the normal of the triangle
 * @param t the triangle
 * @return the normal vector
 */
Vector3D gfc_triangle_get_normal(Triangle3D t);

/**
 * @brief check if the edge intersects the triangle
 * @param e the edge to test with (from a to b)
 * @param t the triangle to test
 * @param contact [optional] if provided it will be populated with the point of collision
 * @return 0 if no intersection, 1 if there is
 */
Uint8 gfc_triangle_edge_test(
  Edge3D e,
  Triangle3D t,
  Vector3D *contact);

/**
 * @brief determine if the 3D edge intersects the sphere and find the point of contact and normal at contact point
 * @param e the edge to test with (from a to b)
 * @param s the sphere to test
 * @param poc [optional] if provided this will populate with the point of collision
 * @param normal [optional] if provided this will populate with the normal at point of collision
 * @note if start of the edge (e.a) is inside the sphere, the POC will be set to e.a and normal will not be set
 * @return 0 if not overlap, 1 if there is
 */
Uint8 gfc_edge3d_to_sphere_intersection(
    Edge3D e,
    Sphere s,
    Vector3D *poc,
    Vector3D *normal);

/**
 * @brief determine if the 3D edge intersects the box and find the point of contact and normal at contact point
 * @param e the edge to test with (from a to b)
 * @param b the box to test
 * @param poc [optional] if provided this will populate with the point of collision
 * @param normal [optional] if provided this will populate with the normal at point of collision
 * @note if start of the edge (e.a) is inside the box, the POC will be set to e.a and normal will not be set
 * @return 1 if the edge intersects the box, 0 otherwise.
 */
Uint8 gfc_edge_box_test(
    Edge3D e,
    Box b,
    Vector3D *poc,
    Vector3D *normal);

#endif

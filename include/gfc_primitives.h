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
 * @brief check if the edge intersects the triangle
 * @param e the edge to test with
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
 * @param e the 3d edge to test with
 * @param s the sphere to test
 * @param poc [optional] if provided this will populate with the point of collision
 * @param normal [optional] if provided this will populate with the normal at point of collision
 * @return 0 if not overlap, 1 if there is
 */
Uint8 gfc_edge3d_to_sphere_intersection(
    Edge3D e,
    Sphere s,
    Vector3D *poc,
    Vector3D *normal);

#endif

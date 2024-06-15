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
    GFC_Vector3D a,b;
}GFC_Edge3D;

typedef struct
{
    GFC_Vector3D a,b,c;
}GFC_Triangle3D;

typedef struct
{
    float x,y,z;   //vector normal of the plane
    float d;       //distance from origin
}GFC_Plane3D;

typedef struct
{
    float x,y,z,r; // position in space, and radius
}GFC_Sphere;

/**
 * @brief an axis sligned bounding box use for quick collision detection
 * can also be used for tracking any cubic shape that will later be able to be rotated
 */
typedef struct
{
    float x,y,z;   //position of corner
    float w,h,d;   // width, height, and depth offsets
}GFC_Box;

typedef enum
{
    GPT_POINT,
    GPT_SPHERE,
    GPT_EDGE,
    GPT_PLANE,
    GPT_TRIANGLE,
    GPT_BOX,
    GPT_MAX
}GFC_PrimitiveTypes;

typedef struct
{
    GFC_PrimitiveTypes type;
    union
    {
        GFC_Vector3D p;//point
        GFC_Sphere s;
        GFC_Edge3D e;
        GFC_Plane3D pl;
        GFC_Triangle3D t;
        GFC_Box b;
    }s;
}GFC_Primitive;

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
GFC_Box gfc_box(float x, float y, float z, float w, float h, float d);


#define gfc_box_cpy(dst,src) (dst.x = src.x,dst.y = src.y,dst.z = src.z,dst.w = src.w,dst.h = src.h,dst.d = src.d)

/**
 * @brief make a sphere based on its component
 * @param x the center x value
 * @param y the center y value
 * @param z the center z value
 * @param r the radius
 * @return a set sphere
 */
GFC_Sphere gfc_sphere(float x, float y, float z, float r);

/**
 * @brief make a plane based on its component
 * @param x the normal x value
 * @param y the normal y value
 * @param z the normal z value
 * @param d the distance from origin
 * @return a set sphere
 */
GFC_Plane3D gfc_plane3d(float x, float y, float z, float d);

/**
 * @brief make a triangle based on 3 points
 * @param a a point
 * @param b a point
 * @param c a point
 * @return a set triangle
 */
GFC_Triangle3D gfc_triangle(GFC_Vector3D a,GFC_Vector3D b,GFC_Vector3D c);

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
GFC_Edge3D gfc_edge3d(float ax,float ay,float az,float bx,float by,float bz);

/**
 * @brief make an edge from component gfc_vectors
 * @param a point a's position
 * @param b point b's potition
 * @return a set edge3d
 */
GFC_Edge3D gfc_edge3d_from_vectors(GFC_Vector3D a,GFC_Vector3D b);

//tests
/**
 * @brief check if a point is within a bounding box
 * @param p the point to check
 * @param b the bounding box to check
 * @return 1 if the point is inside, 0 if not
 */
Uint8 gfc_point_in_box(GFC_Vector3D p,GFC_Box b);

/**
 * @brief check if two boxes overlap
 * @param a one of the boxes
 * @param b the other box
 * @return 1 if there is any overlap, 0 if not
 */
Uint8 gfc_box_overlap(GFC_Box a,GFC_Box b);

/**
 * @brief check if a point is within a sphere
 * @param p the point to check
 * @param s the sphere the check
 * @return 1 if it is inside, 0 if not
 */
Uint8 gfc_point_in_sphere(GFC_Vector3D p,GFC_Sphere s);

/**
 * @brief check if two spheres overlap
 * @param A one sphere
 * @param B the other sphere
 * @return 1 if there is any overlap, 0 if not
 */
Uint8 gfc_sphere_overlap(GFC_Sphere A, GFC_Sphere B);

/**
 * @brief given a triangle, get its plane
 * @param t the triangle in question
 * @return the plane determined by the triangle
 */
GFC_Plane3D gfc_trigfc_angle_get_plane(GFC_Triangle3D t);

/**
 * @brief get the normal of the triangle
 * @param t the triangle
 * @return the normal gfc_vector
 */
GFC_Vector3D gfc_trigfc_angle_get_normal(GFC_Triangle3D t);

/**
 * @brief check if the edge intersects the triangle
 * @param e the edge to test with (from a to b)
 * @param t the triangle to test
 * @param contact [optional] if provided it will be populated with the point of collision
 * @return 0 if no intersection, 1 if there is
 */
Uint8 gfc_trigfc_angle_edge_test(
  GFC_Edge3D e,
  GFC_Triangle3D t,
  GFC_Vector3D *contact);

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
    GFC_Edge3D e,
    GFC_Sphere s,
    GFC_Vector3D *poc,
    GFC_Vector3D *normal);

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
    GFC_Edge3D e,
    GFC_Box b,
    GFC_Vector3D *poc,
    GFC_Vector3D *normal);

/**
 * @brief check if a point is contained within a shape
 * @param point to check
 * @param primitive the shape to check
 * @return 0 if no overlap, 1 if there is
 */
Uint8 gfc_point3d_in_primitive(GFC_Vector3D point, GFC_Primitive primitive);

/**
 * @brief move a shape based on an offset.
 */
GFC_Primitive gfc_primitive_offset(GFC_Primitive primitive,GFC_Vector3D offset);

/**
 * @brief load a primitive from json config.  Json must match the example
 * @param config to parse
 * @return a zero shape or one extracted from config
 * @example:
 * "shape":{"box":{"m":[x,y,z],"s":[w,h,d]}}
 * - or -
 * "shape":{"edge":{"a":[x,y,z],"b":[z,y,z]}}
 * - or -
 * "shape":{"point":[x,y,z]}
 * - etc -
 */
GFC_Primitive gfc_primitive_from_config(SJson *config);

/**
 * @brief load box information from json config.  Json must match the example
 * @param config to parse
 * @return a zero shape or one extracted from config
 * @example:
 * "box":
 * {
 *      "m":[x,y,z],
 *      "s":[w,h,d]
 * }
 */
GFC_Box gfc_box_from_config(SJson *config);

/**
 * @brief load sphere information from json config.  Json must match the example
 * @param config to parse
 * @return a zero shape or one extracted from config
 * @example:
 * "sphere":
 * {
 *      "c":[x,y,z],
 *      "r":d
 * }
 */
GFC_Sphere gfc_sphere_from_config(SJson *config);

/**
 * @brief load plane information from json config.  Json must match the example
 * @param config to parse
 * @return a zero shape or one extracted from config
 * @example:
 * "plane":
 * {
 *      "n":[x,y,z],
 *      "d":d
 * }
 */
GFC_Plane3D gfc_plane_from_config(SJson *config);

/**
 * @brief load triangle information from json config.  Json must match the example
 * @param config to parse
 * @return a zero shape or one extracted from config
 * @example:
 * "triangle":
 * {
 *      "a":[x,y,z],
 *      "b":[x,y,z],
 *      "c":[x,y,z]
 * }
 */
GFC_Triangle3D gfc_triangle_from_config(SJson *config);

/**
 * @brief load edge information from json config.  Json must match the example
 * @param config to parse
 * @return a zero shape or one extracted from config
 * @example:
 * "edge":
 * {
 *      "a":[x,y,z],
 *      "b":[x,y,z]
 * }
 */
GFC_Edge3D gfc_edge_from_config(SJson *config);

#endif

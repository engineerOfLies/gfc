#ifndef __GFC_SHAPE_H__
#define __GFC_SHAPE_H__

#include <SDL.h>

#include "simple_json.h"

#include "gfc_vector.h"
#include "gfc_color.h"
#include "gfc_list.h"


/**
 * @purpose gfc_shapes is meant to provide a common way to represent simple 2D shapes and test collisions based on them
 */

typedef struct
{
    double x1,y1,x2,y2;
}GFC_Edge2D;

typedef struct
{
    double       x,y,r;
}GFC_Circle;

typedef struct
{
    double x,y,w,h;
}GFC_Rect;

typedef enum
{
    ST_RECT,
    ST_CIRCLE,
    ST_EDGE
}GFC_ShapeTypes;


typedef struct
{
    GFC_ShapeTypes type;
    union
    {
        GFC_Circle c;
        GFC_Rect r;
        GFC_Edge2D e;
    }s;
}GFC_Shape;

/**
 * @brief macro to set an sdl rect.  should work with any data structure with elements x,y,w,h
 * @param r the rect to set
 * @param a x param
 * @param b y param
 * @param c w param
 * @param d h param
 */
#define gfc_rect_set(r,a,b,c,d) (r.x=(a), r.y=(b), r.w=(c), r.h=(d))

/**
 * @brief convenience function to return a made SDL_Rect based on provided parameters.
 * @note this may be made redundant by recent SDL Updates
 * @param x left
 * @param y right
 * @param w width
 * @param h height
 */
SDL_Rect gfc_sdl_rect(Sint32 x,Sint32 y,Uint32 w, Uint32 h);

/**
 * @brief check if two shapes are exactly the same
 * @param a one shape to check
 * @param b one shape to check
 * @return 0 if there are any differences, 1 if exactly the same
 */
Uint8 gfc_shape_compare(GFC_Shape a, GFC_Shape b);

/**
 * @brief parse a shape out of json
 * @param json the json object to parse
 * @param shape pointer to the shape to store the information into
 * @return 0 on error or 1 on success
 */
int gfc_shape_from_json(SJson *json,GFC_Shape *shape);

/**
 * @brief get rect information out of a json value
 * @param json the json to parse
 * @param rect [output] save the results here
 * @return false on failure, true on success
 */
int gfc_rect_from_json(SJson *json,GFC_Rect *rect);

/**
 * @brief make a GFC GFC_Rect
 * @param x the left position of the rect
 * @param y the top position of the rect
 * @param w the width of the rect
 * @param h the height of the rect
 * @return a GFC rect
 */
GFC_Rect gfc_rect(float x, float y, float w, float h);

/**
 * @brief make a GFC GFC_Rect
 * @param v the gfc_vector holding rect info
 * @return a GFC rect
 */
GFC_Rect gfc_rect_from_vector4(GFC_Vector4D v);

/**
 * @brief make a shape based on a rect
 * @param x the left side
 * @param y the top of the rect
 * @param w the width
 * @param h the height
 */
GFC_Shape gfc_shape_rect(float x, float y, float w, float h);

/**
 * @brief convert a rect to a gfc_vector4d
 * @param r the rect to convert
 * @returns a gfc_vector4f
 */
GFC_Vector4D gfc_rect_to_vector4d(GFC_Rect r);

/**
 * @brief make a shape based on a gfc rect
 * @param r the rect to base it on
 */
GFC_Shape gfc_shape_from_rect(GFC_Rect r);

/**
 * @brief make a shape based on a SDL rect
 * @param r the rect to base it on
 */
GFC_Shape gfc_shape_from_sdl_rect(SDL_Rect r);

/**
 * @brief make a shape based on a circle
 * @param x the center x
 * @param y the center y
 * @param r the radius
 */
GFC_Shape gfc_shape_circle(float x, float y, float r);

/**
 * @brief make a shape based on a gfc GFC_Circle
 * @param c the circle to make the shape with
 * @return the shape
 */
GFC_Shape gfc_shape_from_circle(GFC_Circle c);

/**
 * @brief get a circle from the shape
 * @param s the shape to get the cirlce from
 * @return the circle
 */
GFC_Circle gfc_shape_to_circle(GFC_Shape s);

/**
 * @brief make an edge shape basesd on the points provided
 * @param x1 the X component of starting point
 * @param y1 the Y component of starting point
 * @param x2 the X component of ending point
 * @param y2 the Y component of ending point
 * @return the shape
 */
GFC_Shape gfc_shape_edge(float x1,float y1,float x2,float y2);

/**
 * @brief make a shape based on a gfc GFC_Edge2D
 * @param e the edge to make the shape with
 * @return the shape
 */
GFC_Shape gfc_shape_from_edge(GFC_Edge2D e);

/**
 * @brief set all parameters of a GFC rect at once
 * @param r the rectangle to set
 * @param a the x component
 * @param b the y component
 * @param c the width
 * @param d the height
 */
#ifndef gfc_rect_set
#define gfc_rect_set(r,a,b,c,d) (r.x = a,r.y = b, r.w =c, r.h = d)
#endif

/**
 * @brief copy the contents of one rect into another
 * @param dst the rect to be copied into
 * @param src the rect to be copied from
 */
#define gfc_rect_copy(dst,src) (dst.x = src.x,dst.y = src.y,dst.w = src.w,dst.h = src.h)

/**
 * @brief determine if the point lies within the rectangle
 * @param p the point to check
 * @param r the rectangle to check
 * @return true if the point is inside the rectangle, false otherwise
 */
Uint8 gfc_point_in_rect(GFC_Vector2D p,GFC_Rect r);

/**
 * @brief check if two rectangles are overlapping
 * @param a rect A
 * @param b rect B
 * @return true if there is any overlap, false otherwise
 */
Uint8 gfc_rect_overlap(GFC_Rect a,GFC_Rect b);

/**
 * @brief check if two rectangles are overlapping
 * @param a rect A
 * @param b rect B
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @return true if there is any overlap, false otherwise
 */
Uint8 gfc_rect_overlap_poc(GFC_Rect a,GFC_Rect b,GFC_Vector2D *poc, GFC_Vector2D *normal);


/**
 * @brief make a GFC GFC_Circle
 * @param x the position of the circle center
 * @param y the position of the circle center
 * @param r the radius of the circle
 */
GFC_Circle gfc_circle(float x, float y, float r);

/**
 * @brief get the bounding circle for the given rectangle
 * @param r the rectangle
 * @return the circle who's radius intersects the corners of the rectangle
 */
GFC_Circle gfc_rect_get_bounding_circle(GFC_Rect r);

/**
 * @brief get the bounding circle for the given edge
 * @param e the edge
 * @return the circle who's radius intersects the endpoints of the edge
 */
GFC_Circle gfc_edge_get_bounding_circle(GFC_Edge2D e);

/**
 * @brief set all the parameters of a GFC circle at once
 * @param
 */
#define gfc_circle_set(circle,a,b,c) (circle.x = a,circle.y = b, circle.r = c)

/**
 * @brief check if the point lies within the circle c
 * @param p the point to check
 * @param c the circle to check
 * @return true if the point is in the circle, false otherwise
 */
Uint8 gfc_point_in_cicle(GFC_Vector2D p,GFC_Circle c);

/**
 * @brief check if two circles are overlapping
 * @param a circle A
 * @param b circle B
 * @param returns true is there is overlap, false otherwise
 */
Uint8 gfc_circle_overlap(GFC_Circle a, GFC_Circle b);

/**
 * @brief check if two circles are overlapping and get the point of contact
 * @param a circle A
 * @param b circle B
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @param returns true is there is overlap, false otherwise
 */
Uint8 gfc_circle_overlap_poc(GFC_Circle a, GFC_Circle b,GFC_Vector2D *poc,GFC_Vector2D *normal);

/**
 * @brief check if two circle intersect and get the points of intersection
 * @note this is more math heavy than the other circle overlap functions
 * @param A first circle to check
 * @param B second circle to check
 * @param pocA (optional) if you want the first point of collision, provide a pointer to a gfc_vector here
 * @param pocB (optional) if you want the second point of collision, provide a pointer to a gfc_vector here
 * @return -1 if the circles are the same, 0 if there are no common points of intersection, 
 *          1 if there is a single point of intersection or 2 if there are both.
 * @note in the event this returns 1 both pocA and pocB will be the same point
 */
int gfc_circle_intersect_circle(GFC_Circle A, GFC_Circle B, GFC_Vector2D *pocA, GFC_Vector2D *pocB);

/**
 * @brief check if a circle and rect overlap
 * @param a the GFC_Circle
 * @param b the GFC_Rect
 * @return true if there is any overlap, false otherwise
 */
Uint8 gfc_circle_rect_overlap(GFC_Circle a, GFC_Rect b);

/**
 * @brief check if a circle and rect overlap
 * @param a the GFC_Circle
 * @param b the GFC_Rect
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @return true if there is any overlap, false otherwise
 */
Uint8 gfc_circle_rect_overlap_poc(GFC_Circle a, GFC_Rect b,GFC_Vector2D *poc,GFC_Vector2D * normal);

/**
 * @brief check if a point is inside a shape
 * @note edges always return NULL
 * @param p the point to test
 * @param s the shape to test
 * @return 0 if not, 1 if it is
 */
Uint8 gfc_point_in_shape(GFC_Vector2D p,GFC_Shape s);

/**@brief check if a shape is overlapping another shape
 * @param a one shape
 * @param b the other shape
 * @return true is there is overlap, false otherwise
 */
Uint8 gfc_shape_overlap(GFC_Shape a, GFC_Shape b);

/**@brief check if a shape is overlapping another shape
 * @param a one shape
 * @param b the other shape
 * @return true is there is overlap, false otherwise
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 */
Uint8 /**/gfc_shape_overlap_poc(GFC_Shape a, GFC_Shape b, GFC_Vector2D *poc, GFC_Vector2D *normal);

/**
 * @brief convert a GFC rect to an SDL rect
 * @param r the GFC rect to convert
 * @return an SDL rect
 */
SDL_Rect gfc_rect_to_sdl_rect(GFC_Rect r);

/**
 * @brief convert an SDL GFC_Rect to a GFC rect
 * @param r the SDL GFC_Rect to convert
 * @return a GFC rect
 */
GFC_Rect gfc_rect_from_sdl_rect(SDL_Rect r);

/**
 * @brief change the position of the shape based on the movement gfc_vector
 * @param shape a pointer to the shape to move
 * @param move the amount to move the shape
 */
void gfc_shape_move(GFC_Shape *shape,GFC_Vector2D move);

/**
 * @brief copy one shape into another
 * @param dst a pointer to the shape you want to copy into
 * @param src the shape you want to copy FROM
 */
void gfc_shape_copy(GFC_Shape *dst,GFC_Shape src);

/**
 * @brief make an edge
 * @param x1 the X component of starting point
 * @param y1 the Y component of starting point
 * @param x2 the X component of ending point
 * @param y2 the Y component of ending point
 * @return a set edge
 */
GFC_Edge2D gfc_edge(float x1, float y1, float x2, float y2);

/**
 * @brief return the length of the edge
 * @param e the edge in question
 * @return the length
 */
float gfc_edge_length(GFC_Edge2D e);

/**
 * @brief make an edge from two gfc_vectors
 * @param a the starting point gfc_vector
 * @param b the ending point gfc_vector
 * @return a set edge
 */
GFC_Edge2D gfc_edge_from_vectors(GFC_Vector2D a,GFC_Vector2D b);

/**
 * @brief set an edge
 * @param e the edge to set
 * @param a the X component of starting point
 * @param b the Y component of starting point
 * @param c the X component of ending point
 * @param d the Y component of ending point
 */
#define gfc_edge_set(e,a,b,c,d) (e.x1 = a,e.y1 = b, e.x2 = c, e.y2 = d)

/**
 * @brief copy the contents of the src edge into the dst edge
 * @param dst the destination of the copy
 * @param src the source of the copy
 */
#define gfc_edge_copy(dst,src) (dst.x1 = src.x1,dst.y1 = src.y1,dst.x2 = src.x2,dst.y2 = src.y2)

/**
 * @brief determine if and where two edges intersect
 * @param a edge A
 * @param b edge B
 * @param contact (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal (optional) if provided this will be populated with a gfc_vector perpendicular to b
 * @return true on intersection, false otherwise
 */
Uint8 gfc_edge_intersect_poc(
    GFC_Edge2D a,
    GFC_Edge2D b,
    GFC_Vector2D *contact,
    GFC_Vector2D *normal);

/**
 * @brief determine if and where two edges intersect
 * @param a edge A
 * @param b edge B
 * @return true on intersection, false otherwise
 */
Uint8 gfc_edge_intersect(GFC_Edge2D a,GFC_Edge2D b);

/**
 * @brief check if an edge intersects a rectangle
 * @param e the edge to test
 * @param r the rect to rest
 * @return true if there is an intersection, false otherwise
 */
Uint8 gfc_edge_rect_intersection(GFC_Edge2D e, GFC_Rect r);

/**
 * @brief check if an edge intersects a rectangle and get the point of contact and normal
 * @param e the edge to test
 * @param r the rect to rest
 * @param contact (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal (optional) if provided this will be populated with a gfc_vector perpendicular to b
 * @return true if there is an intersection, false otherwise
 */
Uint8 gfc_edge_rect_intersection_poc(GFC_Edge2D e, GFC_Rect r,GFC_Vector2D *poc,GFC_Vector2D *normal);

/**
 * @brief check if an edge intersects a circle
 * @param e the edge to check
 * @param c the circle to check
 * @return true if there is an intersection, false otherwise
 */
Uint8 gfc_edge_circle_intersection(GFC_Edge2D e,GFC_Circle c);

/**
 * @brief check if an edge intersects a circle and get point of contact
 * @param e the edge to check
 * @param c the circle to check
 * @param poc (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @return true if there is an intersection, false otherwise
 */
Uint8 gfc_edge_circle_intersection_poc(GFC_Edge2D e,GFC_Circle c,GFC_Vector2D *poc,GFC_Vector2D *normal);

/**
 * @brief check if the edge intersects the shape
 * @param e the edge the test
 * @param s the shape to test
 * @param true if the shape and edge intersect, false otherwise
 */
Uint8 gfc_edge_intersect_shape(GFC_Edge2D e,GFC_Shape s);

/**
 * @brief check if the edge intersects the shape
 * @param e the edge the test
 * @param s the shape to test
 * @param poc (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @param true if the shape and edge intersect, false otherwise
 */
Uint8 gfc_edge_intersect_shape_poc(GFC_Edge2D e,GFC_Shape s,GFC_Vector2D *poc,GFC_Vector2D *normal);

/**
 * @brief echo out the shape information to log (and stdout)
 * @param shape the shape information to echo
 */
void gfc_shape_slog(GFC_Shape shape);

void gfc_edge_slog(GFC_Edge2D e);
void gfc_circle_slog(GFC_Circle c);

/**
 * @brief echo out the rect information to log (and stdout)
 * @param r the rect information to echo
 */
void gfc_rect_slog(GFC_Rect r);

/**
 * @brief get the minum rectangle that bounds the shape
 * @param shape the shape to get the bounds of
 * @return the bounding rectangle
 */
GFC_Rect gfc_shape_get_bounds(GFC_Shape shape);

/**
 * @brief get the normal of the shape relative to a reference shape
 * @param s the shape to get the normal from
 * @param s2 the normal should be pointing towards this shape
 * @return an empty gfc_vector if the refPoint is in the shape, a unit gfc_vector otherwise
 */
GFC_Vector2D gfc_shape_get_normal_for_shape(GFC_Shape s, GFC_Shape s2);

/**
 * @brief get the normal of the shape relative to a reference for a given circle
 * @param s the shape to get the normal from
 * @param c the normal should be pointing towards this shape
 * @return an empty gfc_vector if the refPoint is in the shape, a unit gfc_vector otherwise
 */
GFC_Vector2D gfc_shape_get_normal_for_cirlce(GFC_Shape s, GFC_Circle c);

/**
 * @brief get the center point of a rect
 * @param r the rectangle to use
 * @return the center point of the rect
 */
GFC_Vector2D gfc_rect_get_center_point(GFC_Rect r);

/**
 * @brief get the interpolated point along a bezier curve described by the points provided in 2d space
 * @param p0 a point bounding the curve
 * @param p1 a point bounding the curve
 * @param p2 a point bounding the curve
 * @param t the time step along the curve to determine where the point is.  should be between zero and 1
 * @return the position on the curve corresponding to the time step provided
 */
GFC_Vector2D gfc_shape_get_bezier_point_2d(GFC_Vector2D p0, GFC_Vector2D p1, GFC_Vector2D p2,float t);

/**
 * @brief get the interpolated point along a bezier curve described by the points provided in 3d space
 * @param p0 a point bounding the curve
 * @param p1 a point bounding the curve
 * @param p2 a point bounding the curve
 * @param t the time step along the curve to determine where the point is.  should be between zero and 1
 * @return the position on the curve corresponding to the time step provided
 */
GFC_Vector3D gfc_shape_get_bezier_point_3d(GFC_Vector3D p0, GFC_Vector3D p1, GFC_Vector3D p2,float t);

/**
 * @brief get a list of points that describe a bezier curve bound by the 3 points provided in 2D
 * @param p0 a point bounding the curve
 * @param p1 a point bounding the curve
 * @param p2 a point bounding the curve
 * @param count how many points should be in the list
 * @return NULL on error or a list of points for a bezier curve.
 */
GFC_List *gfc_shape_get_bezier_point_list_2d(GFC_Vector2D p0, GFC_Vector2D p1, GFC_Vector2D p2,Uint32 count);

/**
 * @brief get a list of points that describe a bezier curve bound by the 3 points provided in 3D
 * @param p0 a point bounding the curve
 * @param p1 a point bounding the curve
 * @param p2 a point bounding the curve
 * @param count how many points should be in the list
 * @return NULL on error or a list of points for a bezier curve.
 */
GFC_List *gfc_shape_get_bezier_point_list_3d(GFC_Vector3D p0, GFC_Vector3D p1, GFC_Vector3D p2,Uint32 count);

/**
 * @brief free a point list, works for both 2d and 3d
 * @param list the list of points (as created from above) to delete
 * @note the list itself is also deleted
 */
void gfc_shape_point_list_free(GFC_List *list);

#endif

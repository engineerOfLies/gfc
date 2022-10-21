#include "simple_logger.h"

#include "gfc_shape.h"
#include "gfc_primitives.h"

Box gfc_box(float x, float y, float z, float w, float h, float d)
{
    Box b = {x, y, z, w, h, d};
    return b;
}

Sphere gfc_sphere(float x, float y, float z, float r)
{
    Sphere s = {x, y, z, r};
    return s;
}

Plane3D gfc_plane3d(float x, float y, float z, float d)
{
    Plane3D p = {x, y, z, d};
    return p;
}

Uint8 gfc_point_in_sphere(Vector3D p,Sphere s)
{
    if (vector3d_magnitude_compare(
            vector3d(s.x-p.x,s.y-p.y,s.z-p.z),
            s.r) <= 0)return 1;
    return 0;
}

Uint8 gfc_sphere_overlap(Sphere A, Sphere B)
{
    if (vector3d_magnitude_compare(
        vector3d(A.x-B.x,A.y-B.y,A.z-B.z),
        A.r+B.r) <= 0)return 1;
    return 0;
}

Uint8 gfc_box_overlap(Box a,Box b)
{
    if ((a.x > b.x+b.w)||(b.x > a.x+a.w)||
        (a.y > b.y+b.h)||(b.y > a.y+a.h)||
        (a.z > b.z+b.d)||(b.z > a.z+a.d))
    {
        return 0;
    }
    return 1;
}


Vector3D gfc_triangle_get_normal(Triangle3D t)
{
    Vector3D normal = {0};
    normal.x = (t.a.y * (t.b.z - t.c.z)) + (t.b.y * (t.c.z - t.a.z)) + (t.c.y * (t.a.z - t.b.z));
    normal.y = (t.a.z * (t.b.x - t.c.x)) + (t.b.z * (t.c.x - t.a.x)) + (t.c.z * (t.a.x - t.b.x));
    normal.z = (t.a.x * (t.b.y - t.c.y)) + (t.b.x * (t.c.y - t.a.y)) + (t.c.x * (t.a.y - t.b.y));
    vector3d_normalize(&normal);
    return normal;
}

Plane3D gfc_triangle_get_plane(Triangle3D t)
{
    Plane3D p;
    Vector3D normal;
    normal = gfc_triangle_get_normal(t);
    vector3d_copy(p,normal);// pass by name!!!
    p.d = -((t.a.x *(t.b.y * t.c.z - t.c.y * t.b.z)) +
            (t.b.x *(t.c.y * t.a.z - t.a.y * t.c.z)) +
            (t.c.x *(t.a.y * t.b.z - t.b.y * t.a.z)));
    return p;
}

float gfc_edge_in_plane(
    Edge3D e,
    Plane3D p,
    Vector3D *contact)
{
    Vector3D dir;
    float denom,t = 0;
    
    vector3d_sub(dir,e.b,e.a);
    denom = ((p.x * dir.x) + (p.y * dir.y) + (p.z * dir.z));
    if(denom == 0)return 0;
    t = - (((p.x * e.a.x) + (p.y * e.a.y) + (p.z * e.a.z) + p.d) / denom);
    if((t > 0)&&(t <= 1))
    {
        if (contact)
        {
        contact->x = e.a.x + (dir.x * t);
        contact->y = e.a.y + (dir.y * t);
        contact->z = e.a.z + (dir.z * t);
        }
        return t;
    }
    if (contact)
    {
        contact->x = e.a.x + (dir.x * t);
        contact->y = e.a.y + (dir.y * t);
        contact->z = e.a.z + (dir.z * t);
    }
    return t;
}

Box gfc_triangle_get_bounding_box(Triangle3D t)
{
    Box b;
    b.x = MIN(MIN(t.a.x,t.b.x),t.c.x);
    b.y = MIN(MIN(t.a.y,t.b.y),t.c.y);
    b.z = MIN(MIN(t.a.z,t.b.z),t.c.z);
    b.w = MAX(MAX(t.a.x,t.b.x),t.c.x) - b.x;
    b.h = MAX(MAX(t.a.y,t.b.y),t.c.y) - b.y;
    b.d = MAX(MAX(t.a.z,t.b.z),t.c.z) - b.z;
    return b;
}

Uint8 gfc_point_in_triangle(
    Vector3D point,
    Triangle3D t,
    Plane3D p)
{
    float rayTest = 0;
    Uint8 intersectCount = 0;
    
    rayTest = vector3d_magnitude_squared(vector3d(point.x - t.a.x,point.y - t.a.y,point.z - t.a.z)) +
                vector3d_magnitude_squared(vector3d(point.x - t.b.x,point.y - t.b.y,point.z - t.b.z)) +
                vector3d_magnitude_squared(vector3d(point.x - t.c.x,point.y - t.c.y,point.z - t.c.z));
    //The above makes sure that the testing ray is going to be long enought to go through all possible edge of the trianlge
    // if it were somehow too short, it might give a false positive, vector3d_magnitude_squared is fast
  
    if (p.x > (MAX(p.y,p.z)))
    {
        /*project triangle to yz plane*/
        if (gfc_edge_intersect(
            gfc_edge(point.y,point.z,point.y,point.z + rayTest),
            gfc_edge(t.a.y,t.a.z,t.b.y,t.b.z)))//triangle edge
        {
            intersectCount++;
        }
        if (gfc_edge_intersect(
            gfc_edge(point.y,point.z,point.y,point.z + rayTest),
            gfc_edge(t.b.y,t.b.z,t.c.y,t.c.z)))//triangle edge
        {
            intersectCount++;
        }
        if (gfc_edge_intersect(
            gfc_edge(point.y,point.z,point.y,point.z + rayTest),
            gfc_edge(t.c.y,t.c.z,t.a.y,t.a.z)))//triangle edge
        {
            intersectCount++;
        }
    }
    else if (p.y > (MAX(p.x,p.z)))
    {
        if (gfc_edge_intersect(
            gfc_edge(point.x,point.z,point.x,point.z + rayTest),
            gfc_edge(t.a.x,t.a.z,t.b.x,t.b.z)))//triangle edge
        {
            intersectCount++;
        }
        if (gfc_edge_intersect(
            gfc_edge(point.x,point.z,point.x,point.z + rayTest),
            gfc_edge(t.b.x,t.b.z,t.c.x,t.c.z)))//triangle edge
        {
            intersectCount++;
        }
        if (gfc_edge_intersect(
            gfc_edge(point.x,point.z,point.x,point.z + rayTest),
            gfc_edge(t.c.x,t.c.z,t.a.x,t.a.z)))//triangle edge
        {
            intersectCount++;
        }
    }
    else
    {
        if (gfc_edge_intersect(
            gfc_edge(point.x,point.y,point.x,point.y + rayTest),
            gfc_edge(t.a.x,t.a.y,t.b.x,t.b.y)))//triangle edge
        {
            intersectCount++;
        }
        if (gfc_edge_intersect(
            gfc_edge(point.x,point.y,point.x,point.y + rayTest),
            gfc_edge(t.b.x,t.b.y,t.c.x,t.c.y)))//triangle edge
        {
            intersectCount++;
        }
        if (gfc_edge_intersect(
            gfc_edge(point.x,point.y,point.x,point.y + rayTest),
            gfc_edge(t.c.x,t.c.y,t.a.x,t.a.y)))//triangle edge
        {
            intersectCount++;
        }
    }
    if (intersectCount % 2)
    {
        return 1;
    }
    return 0;
}


Uint8 gfc_triangle_edge_test(
  Edge3D e,
  Triangle3D t,
  Vector3D *contact)
{
  float time;
  Plane3D p;
  Vector3D intersectPoint;
  
  p = gfc_triangle_get_plane(t);
  time = gfc_edge_in_plane(e,p,&intersectPoint);
  
  if ((time <= 0)|| (time > 1))
  {
    return 0;
  }
  
  if (gfc_point_in_triangle(intersectPoint,t,p))
  {
    if (contact)
    {
      vector3d_copy((*contact),intersectPoint);
    }
    return 1;
  }
  return 0;
}

/*eol@eof*/

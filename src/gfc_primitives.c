#include "simple_logger.h"

#include "gfc_shape.h"
#include "gfc_primitives.h"


Edge3D gfc_edge3d_from_vectors(Vector3D a,Vector3D b)
{
    Edge3D e = {a,b};
    return e;
}

Edge3D gfc_edge3d(float ax,float ay,float az,float bx,float by,float bz)
{
    Edge3D e = {{ax,ay,az},{bx,by,bz}};
    return e;
}

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

Triangle3D gfc_triangle(Vector3D a,Vector3D b,Vector3D c)
{
    Triangle3D t = {a,b,c};
    return t;
}


Uint8 gfc_point_in_box(Vector3D p,Box b)
{
    if ((p.x >= b.x) && (p.x <= b.x + b.w)&&
        (p.y >= b.y) && (p.y <= b.y + b.h)&&
        (p.z >= b.z) && (p.z <= b.z + b.d))
        return 1;
    return 0;
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

Uint8 gfc_edge_box_test(
    Edge3D e,
    Box b,
    Vector3D *poc,
    Vector3D *normal)
{
    Vector3D contact;
    Vector3D vertices[8];
    
    Box skip = {0};
    
    if (gfc_point_in_box(e.a,b))
    {
        //start inside the box
        if (poc)*poc = e.a;
        return 1;
    }
    if (e.a.x > b.x)skip.x = 1;
    if (e.a.y > b.y)skip.y = 1;
    if (e.a.z > b.z)skip.z = 1;
    if (e.a.x < b.x + b.w)skip.x = 1;
    if (e.a.y < b.y + b.h)skip.y = 1;
    if (e.a.z < b.z + b.d)skip.z = 1;
    
    // test each size of the box
    vertices[0] = vector3d(b.x,b.y,b.z);
    vertices[1] = vector3d(b.x + b.w,b.y,b.z);
    vertices[2] = vector3d(b.x,b.y + b.h,b.z);
    vertices[3] = vector3d(b.x + b.w,b.y + b.h,b.z);
    vertices[4] = vector3d(b.x,b.y,b.z + b.d);
    vertices[5] = vector3d(b.x + b.w,b.y,b.z + b.d);
    vertices[6] = vector3d(b.x,b.y + b.h,b.z + b.d);
    vertices[7] = vector3d(b.x + b.w,b.y + b.h,b.z + b.d);
        
    if (!skip.z) //check against the top side
    {
        if (gfc_edge_in_plane(
                e,
                gfc_triangle_get_plane(
                    gfc_triangle(vertices[4],vertices[5],vertices[6])),
                &contact))
        {
            //check for inclusion
            if ((contact.x >= b.x)&&(contact.x <= b.x + b.w)&&
                (contact.y >= b.y)&&(contact.y >= b.y + b.h))
            {
                //we hit!
                if (poc)*poc = contact;
                if (normal)*normal = vector3d(0,0,1);
                return 1;
            }
        }
    }

    if (!skip.d) //check against the bottom side
    {
        if (gfc_edge_in_plane(
                e,
                gfc_triangle_get_plane(
                    gfc_triangle(vertices[0],vertices[1],vertices[2])),
                &contact))
        {
            //check for inclusion
            if ((contact.x >= b.x)&&(contact.x <= b.x + b.w)&&
                (contact.y >= b.y)&&(contact.y >= b.y + b.h))
            {
                //we hit!
                if (poc)*poc = contact;
                if (normal)*normal = vector3d(0,0,-1);
                return 1;
            }
        }
    }

    if (!skip.y) //check against the front side
    {
        if (gfc_edge_in_plane(
                e,
                gfc_triangle_get_plane(
                    gfc_triangle(vertices[0],vertices[1],vertices[4])),
                &contact))
        {
            //check for inclusion
            if ((contact.x >= b.x)&&(contact.x <= b.x + b.w)&&
                (contact.z >= b.z)&&(contact.z >= b.z + b.d))
            {
                //we hit!
                if (poc)*poc = contact;
                if (normal)*normal = vector3d(0,-1,0);
                return 1;
            }
        }
    }

    if (!skip.h) //check against the back side
    {
        if (gfc_edge_in_plane(
                e,
                gfc_triangle_get_plane(
                    gfc_triangle(vertices[2],vertices[3],vertices[6])),
                &contact))
        {
            //check for inclusion
            if ((contact.x >= b.x)&&(contact.x <= b.x + b.w)&&
                (contact.z >= b.z)&&(contact.z >= b.z + b.d))
            {
                //we hit!
                if (poc)*poc = contact;
                if (normal)*normal = vector3d(0,1,0);
                return 1;
            }
        }
    }

    if (!skip.x) //check against the left side
    {
        if (gfc_edge_in_plane(
                e,
                gfc_triangle_get_plane(
                    gfc_triangle(vertices[0],vertices[2],vertices[4])),
                &contact))
        {
            //check for inclusion
            if ((contact.y >= b.y)&&(contact.y <= b.y + b.h)&&
                (contact.z >= b.z)&&(contact.z >= b.z + b.d))
            {
                //we hit!
                if (poc)*poc = contact;
                if (normal)*normal = vector3d(-1,0,0);
                return 1;
            }
        }
    }
    if (!skip.w) //check against the right side
    {
        if (gfc_edge_in_plane(
                e,
                gfc_triangle_get_plane(
                    gfc_triangle(vertices[1],vertices[6],vertices[3])),
                &contact))
        {
            //check for inclusion
            if ((contact.y >= b.y)&&(contact.y <= b.y + b.h)&&
                (contact.z >= b.z)&&(contact.z >= b.z + b.d))
            {
                //we hit!
                if (poc)*poc = contact;
                if (normal)*normal = vector3d(1,0,0);
                return 1;
            }
        }
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

Uint8 gfc_edge3d_to_sphere_intersection(Edge3D e,Sphere s,Vector3D *poc,Vector3D *normal)
{
    float dx, dy, dz, A, B, C, det, t,t1,t2;
    Vector3D intersection1, intersection2;
    Vector3D cp;
    
    if (gfc_point_in_sphere(e.a,s))
    {
        //edge starts in sphere
        if (poc)*poc = e.a;
        return 1;
    }
    dx = e.b.x - e.a.x;
    dy = e.b.y - e.a.y;
    dz = e.b.z - e.a.z;

    cp.x = s.x;
    cp.y = s.y;
    cp.z = s.y;
    A = dx * dx + dy * dy + dz * dz;
    B = 2 * (dx * (e.a.x - s.x) + dy * (e.a.y - s.y) + dz * (e.a.z - s.z));
    C = (e.a.x - s.x) * (e.a.x - s.x) +
        (e.a.y - s.y) * (e.a.y - s.y) +
        (e.a.z - s.z) * (e.a.z - s.z) -
        s.r * s.r;

    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0))
    {
        // No real solutions.
        return 0;
    }
    else if (det == 0)
    {
        // One solution.
        t = -B / (2 * A);
        intersection1 = vector3d(e.a.x + t * dx, e.a.y + t * dy,e.a.z + t * dz);
        
        if ((intersection1.x < MIN(e.a.x,e.b.x))||(intersection1.x > MAX(e.a.x,e.b.x))||
            (intersection1.y < MIN(e.a.y,e.b.y))||(intersection1.y > MAX(e.a.y,e.b.y))||
            (intersection1.z < MIN(e.a.z,e.b.z))||(intersection1.z > MAX(e.a.z,e.b.z)))
        {
            //point lies outside of line segment
            return 0;
        }
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            vector3d_sub(intersection2,cp,intersection1);
            vector3d_normalize(&intersection2);
            *normal = intersection2;
        }
        return 1;
    }
    else
    {
        // Two solutions. picking the one closer to the first point of the edge
        t1 = (float)((-B + sqrt(det)) / (2 * A));
        t2 = (float)((-B - sqrt(det)) / (2 * A));
        intersection1 = vector3d(e.a.x + t1 * dx, e.a.y + t1 * dy, e.a.z + t1 * dz);
        intersection2 = vector3d(e.a.x + t2 * dx, e.a.y + t2 * dy, e.a.z + t2 * dz);
        
        if ((intersection1.x < MIN(e.a.x,e.b.x))||(intersection1.x > MAX(e.a.x,e.b.x))||
            (intersection1.y < MIN(e.a.y,e.b.y))||(intersection1.y > MAX(e.a.y,e.b.y))||
            (intersection1.z < MIN(e.a.z,e.b.z))||(intersection1.z > MAX(e.a.z,e.b.z)))
        {
            if ((intersection2.x < MIN(e.a.x,e.b.x))||(intersection2.x > MAX(e.a.x,e.b.x))||
                (intersection2.y < MIN(e.a.y,e.b.y))||(intersection2.y > MAX(e.a.y,e.b.y))||
                (intersection2.z < MIN(e.a.z,e.b.z))||(intersection2.z > MAX(e.a.z,e.b.z)))
            {
                return 0;
            }
            t = t2;
        }
        else
        {
            if ((intersection2.x < MIN(e.a.x,e.b.x))||(intersection2.x > MAX(e.a.x,e.b.x))||
                (intersection2.y < MIN(e.a.y,e.b.y))||(intersection2.y > MAX(e.a.y,e.b.y))||
                (intersection2.z < MIN(e.a.z,e.b.z))||(intersection2.z > MAX(e.a.z,e.b.z)))
            {
                t = t1;
            }
            else
            {
                t = MIN(t1,t2);
            }
        }
        intersection1 = vector3d(e.a.x + t * dx, e.a.y + t * dy, e.a.z + t * dz);        
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            vector3d_sub(intersection2,cp,intersection1);
            vector3d_normalize(&intersection2);
            *normal = intersection2;
        }
        return 2;
    }
}

/*eol@eof*/

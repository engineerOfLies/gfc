#include "simple_logger.h"

#include "gfc_config.h"
#include "gfc_shape.h"
#include "gfc_primitives.h"


GFC_Edge3D gfc_edge3d_from_vectors(GFC_Vector3D a,GFC_Vector3D b)
{
    GFC_Edge3D e = {a,b};
    return e;
}

GFC_Edge3D gfc_edge3d(float ax,float ay,float az,float bx,float by,float bz)
{
    GFC_Edge3D e = {{ax,ay,az},{bx,by,bz}};
    return e;
}

GFC_Box gfc_box(float x, float y, float z, float w, float h, float d)
{
    GFC_Box b = {x, y, z, w, h, d};
    return b;
}

GFC_Sphere gfc_sphere(float x, float y, float z, float r)
{
    GFC_Sphere s = {x, y, z, r};
    return s;
}

GFC_Plane3D gfc_plane3d(float x, float y, float z, float d)
{
    GFC_Plane3D p = {x, y, z, d};
    return p;
}

GFC_Triangle3D gfc_triangle(GFC_Vector3D a,GFC_Vector3D b,GFC_Vector3D c)
{
    GFC_Triangle3D t = {a,b,c};
    return t;
}


Uint8 gfc_point_in_box(GFC_Vector3D p,GFC_Box b)
{
    if ((p.x >= b.x) && (p.x <= b.x + b.w)&&
        (p.y >= b.y) && (p.y <= b.y + b.h)&&
        (p.z >= b.z) && (p.z <= b.z + b.d))
        return 1;
    return 0;
}

Uint8 gfc_point_in_sphere(GFC_Vector3D p,GFC_Sphere s)
{
    if (gfc_vector3d_magnitude_compare(
            gfc_vector3d(s.x-p.x,s.y-p.y,s.z-p.z),
            s.r) <= 0)return 1;
    return 0;
}

Uint8 gfc_sphere_overlap(GFC_Sphere A, GFC_Sphere B)
{
    if (gfc_vector3d_magnitude_compare(
        gfc_vector3d(A.x-B.x,A.y-B.y,A.z-B.z),
        A.r+B.r) <= 0)return 1;
    return 0;
}

Uint8 gfc_box_overlap(GFC_Box a,GFC_Box b)
{
    if ((a.x > b.x+b.w)||(b.x > a.x+a.w)||
        (a.y > b.y+b.h)||(b.y > a.y+a.h)||
        (a.z > b.z+b.d)||(b.z > a.z+a.d))
    {
        return 0;
    }
    return 1;
}


GFC_Vector3D gfc_trigfc_angle_get_normal(GFC_Triangle3D t)
{
    GFC_Vector3D normal = {0};
    GFC_Vector3D side1,side2;
    gfc_vector3d_sub(side1,t.b,t.a);
    gfc_vector3d_sub(side2,t.c,t.a);

    gfc_vector3d_cross_product(&normal,side1,side2);
    gfc_vector3d_normalize(&normal);
    return normal;
}

GFC_Plane3D gfc_trigfc_angle_get_plane(GFC_Triangle3D t)
{
    GFC_Plane3D p;
    GFC_Vector3D normal;
    normal = gfc_trigfc_angle_get_normal(t);
    gfc_vector3d_copy(p,normal);// pass by name!!!
    p.d = gfc_vector3d_dot_product(t.a,normal);
    return p;
}

float gfc_edge_in_plane(
    GFC_Edge3D e,
    GFC_Plane3D p,
    GFC_Vector3D *contact)
{
    GFC_Vector3D dir,normal;
    float denom,t = 0;
    float distance;
        
    gfc_vector3d_sub(dir,e.b,e.a);//direction from a to b
    distance = gfc_vector3d_magnitude(dir);
    gfc_vector3d_normalize(&dir);
//     slog("test edge start: %f,%f,%f",gfc_vector3d_to_slog(e.a));
//     slog("test edge dir: %f,%f,%f",gfc_vector3d_to_slog(dir));
//     slog("test edge length: %f",distance);
//     slog("test plane: %f,%f,%f, D:%f",gfc_vector3d_to_slog(p),p.d);
    if (!distance)return 0;//zero length edge
    normal = gfc_vector3d(p.x,p.y,p.z);
    //check if parallel
    denom = gfc_vector3d_dot_product(normal,dir);
    if (denom == 0)return 0;// parallel
    
    // Compute the t value for the directed line ray intersecting the plane
    
    t = (p.d - gfc_vector3d_dot_product(normal, e.a)) / denom;
    
//     slog("t value calculated:%f",t);
    
    if (contact)
    {
        contact->x = e.a.x + (dir.x * t);
        contact->y = e.a.y + (dir.y * t);
        contact->z = e.a.z + (dir.z * t);
    }
    t /= distance;
    return t;
}

GFC_Box gfc_trigfc_angle_get_bounding_box(GFC_Triangle3D t)
{
    GFC_Box b;
    b.x = MIN(MIN(t.a.x,t.b.x),t.c.x);
    b.y = MIN(MIN(t.a.y,t.b.y),t.c.y);
    b.z = MIN(MIN(t.a.z,t.b.z),t.c.z);
    b.w = MAX(MAX(t.a.x,t.b.x),t.c.x) - b.x;
    b.h = MAX(MAX(t.a.y,t.b.y),t.c.y) - b.y;
    b.d = MAX(MAX(t.a.z,t.b.z),t.c.z) - b.z;
    return b;
}



Uint8 gfc_point_same_side(GFC_Vector3D p1,GFC_Vector3D p2,GFC_Vector3D a,GFC_Vector3D b)
{
    float f;
    GFC_Vector3D cp1,cp2;
    GFC_Vector3D side1,side2,ref;
    gfc_vector3d_sub(side1,b,a);
    gfc_vector3d_sub(side2,p2,a);
    gfc_vector3d_sub(ref,p1,a);
    gfc_vector3d_cross_product(&cp1,side1,side2);
    gfc_vector3d_cross_product(&cp2,side1, ref);
    f = gfc_vector3d_dot_product(cp1, cp2);
    if (f >= 0)return 1;
    return 0;
}

Uint8 gfc_point_in_triangle(
    GFC_Vector3D point,
    GFC_Triangle3D t)
{
                    //test point, reference point, edge A, edge B
    if (gfc_point_same_side(point,t.c,t.a,t.b) &&
        gfc_point_same_side(point,t.a,t.b,t.c) &&
        gfc_point_same_side(point,t.b,t.c,t.a))
    {
        return 1;
    }
    return 0;
}

Uint8 gfc_point_in_triangle_old(
    GFC_Vector3D point,
    GFC_Triangle3D t,
    GFC_Plane3D p)
{
    GFC_Vector3D absNormal;
    float rayTest = 0;
    Uint8 intersectCount = 0;
    
    rayTest = gfc_vector3d_magnitude_squared(gfc_vector3d(point.x - t.a.x,point.y - t.a.y,point.z - t.a.z)) +
                gfc_vector3d_magnitude_squared(gfc_vector3d(point.x - t.b.x,point.y - t.b.y,point.z - t.b.z)) +
                gfc_vector3d_magnitude_squared(gfc_vector3d(point.x - t.c.x,point.y - t.c.y,point.z - t.c.z));
    //The above makes sure that the testing ray is going to be long enought to go through all possible edge of the trianlge
    // if it were somehow too short, it might give a false positive, gfc_vector3d_magnitude_squared is fast
                
    gfc_vector3d_set(absNormal,abs(p.x),abs(p.y),abs(p.z));
  
    if (absNormal.x > (MAX(absNormal.y,absNormal.z)))
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
    else if (absNormal.y > (MAX(absNormal.x,absNormal.z)))
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
    GFC_Edge3D e,
    GFC_Box b,
    GFC_Vector3D *poc,
    GFC_Vector3D *normal)
{
    GFC_Vector3D contact;
    GFC_Vector3D vertices[8];
    float bestDistance = -1;
    float distance;
    GFC_Vector3D bestContact;
    GFC_Vector3D bestNormal;
        
    if (gfc_point_in_box(e.a,b))
    {
        //start inside the box
        if (poc)*poc = e.a;
        return 1;
    }
    
    // corners of the box
    vertices[0] = gfc_vector3d(b.x,b.y,b.z);            //min corner
    vertices[1] = gfc_vector3d(b.x + b.w,b.y,b.z);
    vertices[2] = gfc_vector3d(b.x,b.y + b.h,b.z);
    vertices[3] = gfc_vector3d(b.x + b.w,b.y + b.h,b.z);
    vertices[4] = gfc_vector3d(b.x,b.y,b.z + b.d);
    vertices[5] = gfc_vector3d(b.x + b.w,b.y,b.z + b.d);
    vertices[6] = gfc_vector3d(b.x,b.y + b.h,b.z + b.d);
    vertices[7] = gfc_vector3d(b.x + b.w,b.y + b.h,b.z + b.d);//max corner
        
    if (gfc_edge_in_plane(
            e,
            gfc_trigfc_angle_get_plane(
                gfc_triangle(vertices[4],vertices[5],vertices[6])),//bottom face
            &contact))
    {
        //check for inclusion
        if (gfc_point_in_rect(gfc_vector2d(contact.x,contact.y),gfc_rect(b.x,b.y,b.w,b.h)))
        {
            //we hit!
            gfc_vector3d_copy(bestContact,contact);
            bestDistance = gfc_vector3d_magnitude_between(e.a,contact);
            bestNormal = gfc_vector3d(0,0,-1);
        }
    }
    if (gfc_edge_in_plane(
            e,
            gfc_trigfc_angle_get_plane(
                gfc_triangle(vertices[0],vertices[1],vertices[2])),//top face
            &contact))
    {
        //check for inclusion
        if (gfc_point_in_rect(gfc_vector2d(contact.x,contact.y),gfc_rect(b.x,b.y,b.w,b.h)))
        {
            //we hit!
            distance = gfc_vector3d_magnitude_between(e.a,contact);
            if ((bestDistance == -1)||(distance < bestDistance))
            {
                gfc_vector3d_copy(bestContact,contact);
                bestNormal = gfc_vector3d(0,0,1);
                bestDistance = distance;
            }
        }
    }
    if (gfc_edge_in_plane(
            e,
            gfc_trigfc_angle_get_plane(
                gfc_triangle(vertices[0],vertices[4],vertices[2])),//left face
            &contact))
    {
        //check for inclusion
        if (gfc_point_in_rect(gfc_vector2d(contact.z,contact.y),gfc_rect(b.z,b.y,b.d,b.h)))
        {
            //we hit!
            distance = gfc_vector3d_magnitude_between(e.a,contact);
            if ((bestDistance == -1)||(distance < bestDistance))
            {
                gfc_vector3d_copy(bestContact,contact);
                bestNormal = gfc_vector3d(-1,0,0);
                bestDistance = distance;
            }
        }
    }
    if (gfc_edge_in_plane(
            e,
            gfc_trigfc_angle_get_plane(
                gfc_triangle(vertices[1],vertices[3],vertices[5])),//right face
            &contact))
    {
        //check for inclusion
        if (gfc_point_in_rect(gfc_vector2d(contact.z,contact.y),gfc_rect(b.z,b.y,b.d,b.h)))
        {
            //we hit!
            distance = gfc_vector3d_magnitude_between(e.a,contact);
            if ((bestDistance == -1)||(distance < bestDistance))
            {
                gfc_vector3d_copy(bestContact,contact);
                bestNormal = gfc_vector3d(1,0,0);
                bestDistance = distance;
            }
        }
    }
    if (gfc_edge_in_plane(
            e,
            gfc_trigfc_angle_get_plane(
                gfc_triangle(vertices[0],vertices[1],vertices[4])),//back face
            &contact))
    {
        //check for inclusion
        if (gfc_point_in_rect(gfc_vector2d(contact.x,contact.z),gfc_rect(b.x,b.z,b.w,b.d)))
        {
            //we hit!
            distance = gfc_vector3d_magnitude_between(e.a,contact);
            if ((bestDistance == -1)||(distance < bestDistance))
            {
                gfc_vector3d_copy(bestContact,contact);
                bestNormal = gfc_vector3d(0,-1,0);
                bestDistance = distance;
            }
        }
    }
    if (gfc_edge_in_plane(
            e,
            gfc_trigfc_angle_get_plane(
                gfc_triangle(vertices[2],vertices[3],vertices[6])),//front face
            &contact))
    {
        //check for inclusion
        if (gfc_point_in_rect(gfc_vector2d(contact.x,contact.z),gfc_rect(b.x,b.z,b.w,b.d)))
        {
            //we hit!
            distance = gfc_vector3d_magnitude_between(e.a,contact);
            if ((bestDistance == -1)||(distance < bestDistance))
            {
                gfc_vector3d_copy(bestContact,contact);
                bestNormal = gfc_vector3d(0,1,0);
                bestDistance = distance;
            }
        }
    }
    if (bestDistance == -1)return 0;//never found a hit
    if (poc)gfc_vector3d_copy((*poc),bestContact);
    if (normal)gfc_vector3d_copy((*normal),bestNormal);
    return 1;
}

Uint8 gfc_trigfc_angle_edge_test(
    GFC_Edge3D e,
    GFC_Triangle3D t,
    GFC_Vector3D *contact)
{
    float time;
    GFC_Plane3D p;
    GFC_Vector3D intersectPoint = {0,0,0};

    p = gfc_trigfc_angle_get_plane(t);
    time = gfc_edge_in_plane(e,p,&intersectPoint);
    if ((time <= 0)||(time > 1))
    {
        return 0;
    }
    if (gfc_point_in_triangle(intersectPoint,t))
    {
        if (contact)
        {
            gfc_vector3d_copy((*contact),intersectPoint);
        }
        return 1;
    }
    return 0;
}

Uint8 gfc_edge3d_to_sphere_intersection(GFC_Edge3D e,GFC_Sphere s,GFC_Vector3D *poc,GFC_Vector3D *normal)
{
    float dx, dy, dz, A, B, C, det, t,t1,t2;
    GFC_Vector3D intersection1, intersection2;
    GFC_Vector3D cp;
    
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
        intersection1 = gfc_vector3d(e.a.x + t * dx, e.a.y + t * dy,e.a.z + t * dz);
        
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
            gfc_vector3d_sub(intersection2,cp,intersection1);
            gfc_vector3d_normalize(&intersection2);
            *normal = intersection2;
        }
        return 1;
    }
    else
    {
        // Two solutions. picking the one closer to the first point of the edge
        t1 = (float)((-B + sqrt(det)) / (2 * A));
        t2 = (float)((-B - sqrt(det)) / (2 * A));
        intersection1 = gfc_vector3d(e.a.x + t1 * dx, e.a.y + t1 * dy, e.a.z + t1 * dz);
        intersection2 = gfc_vector3d(e.a.x + t2 * dx, e.a.y + t2 * dy, e.a.z + t2 * dz);
        
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
        intersection1 = gfc_vector3d(e.a.x + t * dx, e.a.y + t * dy, e.a.z + t * dz);        
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            gfc_vector3d_sub(intersection2,cp,intersection1);
            gfc_vector3d_normalize(&intersection2);
            *normal = intersection2;
        }
        return 2;
    }
}

GFC_Primitive gfc_primitive_offset(GFC_Primitive primitive,GFC_Vector3D offset)
{
    GFC_Primitive p;
    memcpy(&p,&primitive,sizeof(GFC_Primitive));
    switch(primitive.type)
    {
        case GPT_POINT:
            gfc_vector3d_add(p.s.p,p.s.p,offset);
            break;
        case GPT_SPHERE:
            gfc_vector3d_add(p.s.s,p.s.s,offset);
            break;
        case GPT_EDGE:
            gfc_vector3d_add(p.s.e.a,p.s.e.a,offset);
            gfc_vector3d_add(p.s.e.b,p.s.e.b,offset);
            break;
        case GPT_PLANE:
            slog("not yet supported");
            break;
        case GPT_TRIANGLE:
            gfc_vector3d_add(p.s.t.a,p.s.t.a,offset);
            gfc_vector3d_add(p.s.t.b,p.s.t.b,offset);
            gfc_vector3d_add(p.s.t.c,p.s.t.c,offset);
            break;
        case GPT_BOX:
            gfc_vector3d_add(p.s.b,p.s.b,offset);
            break;
        default:
            break;
    }
    return p;
}

Uint8 gfc_point3d_in_primitive(GFC_Vector3D point, GFC_Primitive primitive)
{
    switch(primitive.type)
    {
        case GPT_POINT:
            return gfc_vector3d_compare(point,primitive.s.p);
        case GPT_SPHERE:
            return gfc_point_in_sphere(point,primitive.s.s);
        case GPT_EDGE:
            return gfc_edge3d_to_sphere_intersection(
                primitive.s.e,
                gfc_sphere(point.x,point.y,point.z,0),
                NULL,
                NULL);
        case GPT_PLANE:
            slog("not yet supported");
            break;
        case GPT_TRIANGLE:
            slog("not yet supported");
            break;
        case GPT_BOX:
            return gfc_point_in_box(point,primitive.s.b);
        default:
            return 0;
    }
    return 0;
}

/*
 * "edge":
 * {
 *      "a":[x,y,z],
 *      "b":[x,y,z]
 * }
 */
GFC_Edge3D gfc_edge_from_config(SJson *config)
{
    GFC_Edge3D edge = {0};
    if (!config)return edge;
    sj_object_get_vector3d(config,"a",&edge.a);
    sj_object_get_vector3d(config,"b",&edge.b);    
    return edge;
}

/*
 * "triangle":
 * {
 *      "a":[x,y,z],
 *      "b":[x,y,z],
 *      "c":[x,y,z]
 * }
 */
GFC_Triangle3D gfc_triangle_from_config(SJson *config)
{
    GFC_Triangle3D triangle= {0};
    if (!config)return triangle;
    sj_object_get_vector3d(config,"a",&triangle.a);
    sj_object_get_vector3d(config,"b",&triangle.b);
    sj_object_get_vector3d(config,"c",&triangle.c);
    return triangle;
}

/*
 * "plane":
 * {
 *      "n":[x,y,z],
 *      "d":d
 * }
 */
GFC_Plane3D gfc_plane_from_config(SJson *config)
{
    GFC_Vector3D v;
    GFC_Plane3D plane = {0};
    if (!config)return plane;
    sj_object_get_vector3d(config,"n",&v);
    sj_object_get_value_as_float(config,"d",&plane.d);
    gfc_vector3d_copy(plane,v);
    return plane;
}

/*
 * "sphere":
 * {
 *      "c":[x,y,z],
 *      "r":d
 * }
 */
GFC_Sphere gfc_sphere_from_config(SJson *config)
{
    GFC_Vector3D v;
    GFC_Sphere sphere = {0};
    if (!config)return sphere;
    sj_object_get_vector3d(config,"c",&v);
    sj_object_get_value_as_float(config,"r",&sphere.r);
    gfc_vector3d_copy(sphere,v);
    return sphere;
}

/*
 * "box":
 * {
 *      "m":[x,y,z],
 *      "s":[w,h,d]
 * }
 */
GFC_Box gfc_box_from_config(SJson *config)
{
    GFC_Vector3D v;
    GFC_Box box = {0};
    if (!config)return box;
    sj_object_get_vector3d(config,"m",&v);
    gfc_vector3d_copy(box,v);
    sj_object_get_vector3d(config,"s",&v);
    box.w = v.x;
    box.h = v.y;
    box.d = v.z;
    return box;
}

/*
 * "shape":{"box":{"m":[x,y,z],"s":[w,h,d]}}
 * - or -
 * "shape":{"edge":{"a":[x,y,z],"b":[z,y,z]}}
 * - or -
 * "shape":{"point":[x,y,z]}
 */
GFC_Primitive gfc_primitive_from_config(SJson *config)
{
    SJson *shape = NULL;
    GFC_Primitive primitive = {0};
    if (!config)return primitive;
    shape = sj_object_get_value(config,"triangle");
    if (shape)
    {
        primitive.type = GPT_TRIANGLE;
        primitive.s.t = gfc_triangle_from_config(shape);
        return primitive;
    }
    shape = sj_object_get_value(config,"plane");
    if (shape)
    {
        primitive.type = GPT_PLANE;
        primitive.s.pl = gfc_plane_from_config(shape);
        return primitive;
    }
    shape = sj_object_get_value(config,"edge");
    if (shape)
    {
        primitive.type = GPT_EDGE;
        primitive.s.e = gfc_edge_from_config(shape);
        return primitive;
    }
    shape = sj_object_get_value(config,"box");
    if (shape)
    {
        primitive.type = GPT_BOX;
        primitive.s.b = gfc_box_from_config(shape);
        return primitive;
    }
    shape = sj_object_get_value(config,"point");
    if (shape)
    {
        primitive.type = GPT_POINT;
        sj_value_as_vector3d(shape,&primitive.s.p);
        return primitive;
    }
    shape = sj_object_get_value(config,"sphere");
    if (shape)
    {
        primitive.type = GPT_SPHERE;
        primitive.s.s = gfc_sphere_from_config(shape);
        return primitive;
    }
    return primitive;
}
/*eol@eof*/

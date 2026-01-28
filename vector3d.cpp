#include "vector3d.h"

Vector init_vector(double x, double y, double z)
{
    Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vector add_vec(const Vector &a, const Vector &b)
{
    Vector v;
    v = {a.x + b.x, a.y + b.y, a.z + b.z};
    return v;
}

Vector subtract_vec(const Vector &a, const Vector &b)
{
    Vector v;
    v = {a.x - b.x, a.y - b.y, a.z - b.z};
    return v;
}

Vector multiply_vec(const Vector &a, double speed)
{ 
    Vector v;
    v = {a.x * speed, a.y * speed, a.z * speed};
    return v;
}

double length_vec(const Vector &a) 
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z); 
}

Vector normalize_vec(const Vector &a)
{ 
    double length = length_vec(a);
    if (length > 0)
    {
        Vector v;
        v.x = a.x / length;
        v.y = a.y / length;
        v.z = a.z / length;
        return v;
    }
    else
    {
        return init_vector();
    }
}

Vector cross(const Vector& a, const Vector& b)
{
    Vector v;
    v = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
    return v;
}
#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>

/**
 *  a vector contains a location in 3D
 * @field x horizontal axis
 * @field y vertical axis
 * @field z depth axis
 */
struct Vector 
{
    double x;
    double y;
    double z;
};

/**
 * Create and initializes a new Vector
 * @param x x-axis (default 0)
 * @param y y-axis (default 0)
 * @param z z-axis (default 0)
 * @return a new Vector initialized with the provided values.
 */
Vector init_vector(double x = 0, double y = 0, double z = 0);

/**
 * Returns the sum of two vectors
 * @param a the first vector
 * @param b the second vector
 * @return a vector representing the combined displacement
 */
Vector add_vec(const Vector &a, const Vector &b);

/**
 * Returns the difference of two vectors
 * @param a the first vector
 * @param b the second vector
 * @return a direction vector from b to a
 */
Vector subtract_vec(const Vector &a, const Vector &b);

/**
 * Scales a vector for displayment
 * @param a a vector
 * @param speed speed
 * @return a vector with the same direction but a different length.
 */
Vector multiply_vec(const Vector &a, double speed);

/**
 * Calculates length of the vector
 * @param a the vector
 * @return the length of the vector
 */
double length_vec(const Vector &a);

/**
 * Returns a unit vector in the same direction
 * @param a the vector
 * @return a normalized vector
 */
Vector normalize_vec(const Vector &a);

/**
 * Calculates the cross product of two vectors
 * @param a the first vector.
 * @param b the second vector.
 * @return a vector perpendicular to the plane formed by a and b
 */
Vector cross(const Vector& a, const Vector& b);

#endif
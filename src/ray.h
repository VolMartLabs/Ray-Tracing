/**
\file
\brief .h file that contains ray class implementation
*/

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/**
\brief Ray class implementation.

We see ray as a function P(t) = A + tb. Here P is a 3D position along a line in 3D. A is ray origin and b is track direction. Plug in different t and we move along the ray.
*/
class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction, double time = 0.0)
        : orig(origin), dir(direction), tm(time)
    {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    double time() const { return tm; }

    /**
    \brief P(t) = A + tb

    \param t double we use to move along the ray
    */
    point3 at(double t) const {
        return orig + t * dir;
    }

public:
    point3 orig; // ray origin
    vec3 dir; // ray direction
    double tm; // time for movable objects
};

#endif
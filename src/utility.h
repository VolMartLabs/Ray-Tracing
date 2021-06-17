/**
\file
\brief .h file that contains utility functions
*/

#ifndef UTILITY_H
#define UTILITY_H

#include <random>
#include <cmath>
#include <limits>
#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;


const double infinity = std::numeric_limits<double>::infinity(); // mathematical infinity
const double pi = 3.1415926535897932385; // mathematical pi

/**
\brief Gets random double in range [0.0, 1.0]
*/
inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

/**
\brief Returns a random real in [min,max).

\param min minimal value
\param max maximum value
*/
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

/**
\brief Converts degrees to radians.

\param degrees degrees we want to convert
*/
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}


/**
\brief Clamping function. https://en.wikipedia.org/wiki/Clamping_(graphics)

\param x preferred value
\param min minimal possible value
\param max maximum possible value
*/
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

/**
\brief Gets random int in range [min, max]

\param min minimal value
\param max maximum value
*/
inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}


#include "ray.h"
#include "vec3.h"

#endif
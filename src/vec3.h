/**
\file
\brief .h file that contains three-dimensional vector class implementation
*/

#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

/**
\brief Vector class implementation
*/
class vec3 {
public:
    vec3() : e{ 0,0,0 } {}
    vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    /**
    \brief Return x from vector.
    */
    double x() const { return e[0]; }
    /**
    \brief Return y from vector.
    */
    double y() const { return e[1]; }
    /**
    \brief Return z from vector.
    */
    double z() const { return e[2]; }

    /**
    \brief Return true if the vector is close to zero in all dimensions.
    */
    bool near_zero() const {
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    /**
    \brief Operator - makes coordinates of vector negative.
    */
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    /**
    \brief Operator += adds two vector together.

    \param v input vector
    */
    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    /**
    \brief Operator *= multiplies vector by some constant.

    \param t constant
    */
    vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    /**
    \brief Operator /= divides vector by constant t (multiplies it by 1/t which is the same).

    \param t constant
    */
    vec3& operator/=(const double t) {
        return *this *= 1 / t;
    }

    /**
    \brief Gets length of vector.
    */
    double length() const {
        return sqrt(length_squared());
    }

    /**
    \brief Gets length squared of vector.
    */
    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    /**
    \brief Fill vector with 3 random coordinates.
    */
    inline static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    /**
    \brief Fill vector with 3 random coordinates in range (min, max).

    \param min minimum number in range
    \param max maximum number in range
    */
    inline static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

public:
    double e[3];
};

/**
\brief Pick a random point in a unit radius sphere.
*/
vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

/**
\brief Operator << outputs the vector.

\param out standart output stream
\param v vector to output
*/
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

/**
\brief Operator + that adds two vectors together.
*/
inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

/**
\brief Operator - that substracts two vectors together.
*/
inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

/**
\brief Operator * that multiplies two vectors together.
*/
inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

/**
\brief Operator * that multiplies constant and vector.
*/
inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

/**
\brief Operator * that multiplies vector and constant.
*/
inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

/**
\brief Operator / that divides vector by constant.
*/
inline vec3 operator/(vec3 v, double t) {
    return (1 / t) * v;
}

/**
\brief Makes dot out of two vectors.
*/
inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}

/**
\brief Cross product function.
*/
inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

/**
\brief Makes unit vector out of any other vector.
*/
inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

/**
\brief Gives random unit vector.
*/
vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

/**
\brief Returns vector in the same hemisphere as 'normal' vector.

\param normal vector that exists in the same hemisphere we need.
*/
vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

/**
\brief mirrored light reflection. http://paulbourke.net/geometry/reflected/

\param v input vector that gets reflected
\param n unit vector
*/
vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

/**
\brief Snell's Law implementation. https://en.wikipedia.org/wiki/Snell%27s_law

\param uv input vector that gets reflected
\param n normal
\param etai_over_etat eta/eta_prime both are retractive indicies
*/
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) { // According to Snell's Law
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

/**
\brief Random vector in unit disk.
*/
vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

#endif
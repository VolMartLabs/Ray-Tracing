/**
\file
\brief .h file that contains sphere class
*/

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"

/**
\brief Abstract sphere class.
*/
class sphere : public hittable {
public:
    sphere() {}
    sphere(point3 cen, double r) : center(cen), radius(r) {};
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;

private:
    /**
    \brief Gets (u,v) surface coordinates of the ray-object hit point.

      <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
      <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
      <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    \param p a given point on the sphere of radius one, centered at the origin.
    \param u returned value [0,1] of angle around the Y axis from X=-1.
    \param v returned value [0,1] of angle from Y=-1 to Y=+1.
    */
    static void get_sphere_uv(const point3& p, double& u, double& v) {

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
};

/**
\brief Here we going through sphere and check if rays hit this object.

It hits if there are any t that satisfies equation (P−C)*(P−C) = r^2 where P is any point that ray go though, C is center of sphere.

From sphere in 3D equation: x^2+y^2+z^2=R^2.
We want to know if our ray P(t)=A+tb ever hits the sphere anywhere. If it does hit the sphere, 
there is some t for which P(t) satisfies the sphere equation. So we are looking for any t where this is true: (P(t)−C)*(P(t)−C)=r^2
Full form of P: (A+tb−C)*(A+tb−C)=r^2. And we get t2b*b+2tb*(A−C)+(A−C)*(A−C)−r^2=0.

\param r ray that goes through object
\param t_min minimum t(in a ray) which can be counted as a hit
\param t_max maximum t(in a ray) which can be counted as a hit
\param rec bunch of arguments in the struct
*/
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    // b=2h
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    // Check direction towards surfaces
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

/**
\brief Bounding box function for sphere

\param time0 minumum time
\param time1 maximum time
\param output_box output aabb box
*/
bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
}

#endif

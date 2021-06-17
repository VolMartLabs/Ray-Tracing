/**
\file
\brief .h file that contains constant medium implementation
*/

#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "utility.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

/**
\brief Volume of constant density. Medium tells how far the ray has to travel through the volume also determines how likely it is for the ray to make it through. 

As the ray passes through the volume, it may scatter at any point. The denser the volume, the more likely that is. 
The probability that the ray scatters in any small distance delta(L) is: probability=C*delta(L) where C is proportional to the optical density of the volume.
If you go through all the differential equations, for a random number you get a distance where the scattering occurs. If that distance is outside the volume, 
then there is no 'hit'. For a constant volume we just need the density C and the boundary. I’ll use another hittable for the boundary.
*/
class constant_medium : public hittable {
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
        : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(make_shared<isotropic>(a))
    {}

    constant_medium(shared_ptr<hittable> b, double d, color c)
        : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(make_shared<isotropic>(c))
    {}

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        return boundary->bounding_box(time0, time1, output_box);
    }

public:
    shared_ptr<hittable> boundary;
    shared_ptr<material> phase_function;
    double neg_inv_density;
};

/**
\brief Constant medium hit function. See constant_medium class for more info.

Once a ray exits the constant medium boundary, it will continue forever outside the boundary. 
Put another way, it assumes that the boundary shape is convex. So this particular implementation will work for boundaries like boxes or spheres, 
but will not work with toruses or shapes that contain voids.

\param r ray that goes through object
\param t_min minimum t(in a ray) which can be counted as a hit
\param t_max maximum t(in a ray) which can be counted as a hit
\param rec bunch of arguments in the struct
*/
bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    const bool enableDebug = false; // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool debugging = enableDebug && random_double() < 0.00001;

    hit_record rec1, rec2;

    if (!boundary->hit(r, -infinity, infinity, rec1))
        return false;

    if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))
        return false;

    if (debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

    if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    const auto ray_length = r.direction().length();
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const auto hit_distance = neg_inv_density * log(random_double());

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance = " << hit_distance << '\n'
            << "rec.t = " << rec.t << '\n'
            << "rec.p = " << rec.p << '\n';
    }

    rec.normal = vec3(1, 0, 0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat_ptr = phase_function;

    return true;
}

#endif
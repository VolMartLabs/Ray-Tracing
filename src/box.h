/**
\file
\brief .h file that contains box class implementation
*/

#ifndef BOX_H
#define BOX_H

#include "utility.h"

#include "aarect.h"
#include "hittable_list.h"

/**
\brief Box class. Consists of 6 rectangles.
*/
class box : public hittable {
public:
    box() {}
    box(const point3& p0, const point3& p1, shared_ptr<material> ptr);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = aabb(box_min, box_max);
        return true;
    }

public:
    point3 box_min;
    point3 box_max;
    hittable_list sides;
};

/**
\brief Creates a box out of two points.

\param p0 first point
\param p1 second point
\param ptr material pointer
*/
box::box(const point3& p0, const point3& p1, shared_ptr<material> ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

/**
\brief Computes hits.
Since it consists out of rectangles, so it addreses to each side to compute it independently.

\param r ray that goes through object
\param t_min minimum t(in a ray) which can be counted as a hit
\param t_max maximum t(in a ray) which can be counted as a hit
\param rec bunch of arguments in the struct
*/
bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}

#endif
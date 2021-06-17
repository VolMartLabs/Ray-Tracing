/**
\file
\brief .h file that contains hittable objects class implementation
*/

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

/**
\brief Class that contains hittable objects. We use it to create our worlds.
*/
class hittable_list : public hittable {
public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    /**
    \brief Clears all objects from list.
    */
    void clear() { objects.clear(); }
    /**
    \brief Add object to the list.
    */
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(
        double time0, double time1, aabb& output_box) const override;

public:
    std::vector<shared_ptr<hittable>> objects;
};

/**
\brief Here we going through each element of the list and check if rays hit this object and also saving information about this object as well.

\param r ray that goes through object
\param t_min minimum t(in a ray) which can be counted as a hit
\param t_max maximum t(in a ray) which can be counted as a hit
\param rec bunch of arguments in the struct
*/
bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}


/**
\brief Compute bounding box for BVH (Bounding Volume Hierarchies) constuction.
*/
bool hittable_list::bounding_box(double time0, double time1, aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif
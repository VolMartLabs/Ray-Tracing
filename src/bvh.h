/**
\file
\brief .h file that contains Bounding Volume Hierarchies class implementation
*/

#ifndef BVH_H
#define BVH_H

#include <algorithm>

#include "utility.h"

#include "hittable.h"
#include "hittable_list.h"

/**
\brief Bounding Volume Hierarchies class implementation.

It’s really a container, but it can respond to the query “does this ray hit you?”. 
The children pointers are to generic hittables. They can be other bvh_nodes, or spheres, or any other hittable. 
*/
class bvh_node : public hittable {
public:
    bvh_node();

    bvh_node(const hittable_list& list, double time0, double time1)
        : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
    {}

    bvh_node(
        const std::vector<shared_ptr<hittable>>& src_objects,
        size_t start, size_t end, double time0, double time1);

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb box;
};

/**
\brief Sets box from the class.
*/
bool bvh_node::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = box;
    return true;
}

/**
\brief Check whether the box for the node is hit, and if so, check the children and sort out any details.

\param r input ray
\param t_min on minimum time
\param t_max on maximum time
\param rec struct with params
*/
bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

/**
\brief Generic comparator returns true if the first argument is less than the second, given an additional axis index argument.

\param a hittable element a
\param a hittable element b
\param axis axis by which we compare our elements
*/
inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min().e[axis] < box_b.min().e[axis];
}

/**
\brief Compare at x axis.

\param a box1
\param b box2
*/
bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

/**
\brief Compare at y axis.

\param a box1
\param b box2
*/
bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

/**
\brief Compare at z axis.

\param a box1
\param b box2
*/
bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}

/**
\brief Splitting BVH Volumes. 

As the list of objects in a bvh_node gets divided into two sub-lists, the hit function will work. At each node split the list along one axis.
When the list coming in is two elements, function puts one in each subtree and end the recursion. The traversal algorithm should be smooth and not have to check for null pointers, 
so if I just have one element function duplicates it in each subtree.
*/
inline bvh_node::bvh_node(
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1
) {
    auto objects = src_objects; // Create a modifiable array of the source scene objects

    int axis = random_int(0, 2);
    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    }
    else if (object_span == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            left = objects[start];
            right = objects[start + 1];
        }
        else {
            left = objects[start + 1];
            right = objects[start];
        }
    }
    else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        left = make_shared<bvh_node>(objects, start, mid, time0, time1);
        right = make_shared<bvh_node>(objects, mid, end, time0, time1);
    }

    aabb box_left, box_right;

    if (!left->bounding_box(time0, time1, box_left)
        || !right->bounding_box(time0, time1, box_right)
        )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
}

#endif
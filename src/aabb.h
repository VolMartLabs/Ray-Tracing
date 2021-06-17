/**
\file
\brief .h file that contains Axis-Aligned Bounding Boxes class implementation
*/

#ifndef AABB_H
#define AABB_H

#include "utility.h"

/**
\brief Axis-Aligned Bounding Boxes (AABBs) class implementation

https://en.wikipedia.org/wiki/Minimum_bounding_box

In 3D, those boundaries are planes. Where does the ray hit that plane? Remeber ray equation P(t)=A+tb?
That equation applies to all three of the x/y/z coordinates. For example, x(t)=Ax+tbx. This ray hits the plane x=x0 at the t that satisfies this equation: x_0 = Ax + t_0 * b_x
Thus t at that hitpoint is: t_0 = x_0 − A_x * b_x
First let’s look at computing the intervals: t_x_0 = x_0 − A_x / b_x and t_x_1 = x_1 − A_x / b_x. 
One troublesome thing is that perfectly valid rays will have b_x = 0, causing division by zero. Some of those rays are inside the slab, and some are not. So, using min and max should get us the right answers:
t_x_0 = min((x_0−A_x)/ b_x, (x_1 − A_x)/ b_x) and t_x_1 = min((x_0−A_x)/ b_x, (x_1 − A_x)/ b_x).
*/
class aabb {
public:
    aabb() {}
    aabb(const point3& a, const point3& b) { minimum = a; maximum = b; }

    /**
    \brief returns minimum point
    */
    point3 min() const { return minimum; }
    /**
    \brief returns maximum point
    */
    point3 max() const { return maximum; }

    /**
    \brief Andrew Kensler optimized AABB hit method

    \param r input ray for bounding boxes
    \param t_min minimum time
    \param t_max maximum time
    */
    bool hit(const ray& r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.direction()[a];
            auto t0 = (min()[a] - r.origin()[a]) * invD;
            auto t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    point3 minimum;
    point3 maximum;
};

/**
\brief Function for aabb which computes the bounding box of two boxes:
*/
aabb surrounding_box(aabb box0, aabb box1) {
    point3 small(fmin(box0.min().x(), box1.min().x()),
        fmin(box0.min().y(), box1.min().y()),
        fmin(box0.min().z(), box1.min().z()));

    point3 big(fmax(box0.max().x(), box1.max().x()),
        fmax(box0.max().y(), box1.max().y()),
        fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}

#endif
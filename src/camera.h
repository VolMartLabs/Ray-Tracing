/**
\file
\brief .h file that contains camera class implementation
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"

/**
\brief Positionable camera class.
*/
class camera {
public:
    camera(
        point3 lookfrom, // look from point
        point3 lookat, // look at point
        vec3   vup, // angle
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio, // aspect ratio
        double aperture, // https://en.wikipedia.org/wiki/Aperture
        double focus_dist,
        double _time0 = 0, //time1 for movable objects
        double _time1 = 0 //time2 for movable objects
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        // complete orthonormal basis (u,v,w)
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
        time0 = _time0;
        time1 = _time1;
    }

    /**
    \brief Get ray from camera.

    \param s u double that leads ray to particular pixel on x axis (horizontal)
    \param t v double that leads ray to particular pixel on y axis (vertical)
    */
    ray get_ray(double s, double t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y(); // offset from lens

        return ray(
            origin + offset, // origin
            lower_left_corner + s * horizontal + t * vertical - origin - offset, // direction
            random_double(time0, time1)
        );
    }

private:
    point3 origin;
    point3 lower_left_corner; // lower left corner of the camera (origin - horizontal_vector/2 - vertical_vector/2 - vec3(0, 0, focal_length)
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w; // vectors for orthonormal basis
    double lens_radius;
    double time0, time1;  // shutter open/close times
};
#endif
/**
\file
\brief .h file that contains different textures
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include "utility.h"
#include "perlin.h"
#include "c_stb_image.h"

#include<iostream>

/**
\brief Texture abstract class.
*/
class texture {
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};

/**
\brief Ordinary solid color texture, it interacts with a ray just sending its color. Nuff said.
*/
class solid_color : public texture {
public:
    solid_color() {}
    solid_color(color c) : color_value(c) {}

    solid_color(double red, double green, double blue)
        : solid_color(color(red, green, blue)) {}

    virtual color value(double u, double v, const vec3& p) const override {
        return color_value;
    }

private:
    color color_value; // color value
};

/**
\brief If we multiply trig functions in all three dimensions, the sign of that product forms a 3D checker pattern. 
*/
class checker_texture : public texture {
public:
    checker_texture() {}

    checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : even(_even), odd(_odd) {}

    checker_texture(color c1, color c2)
        : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    /**
    \brief Value of trig function multiplication.

    \param u ray direction x axis
    \param v ray direction y axis
    \param p point of texture
    */
    virtual color value(double u, double v, const point3& p) const override {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd; // odd value texture
    shared_ptr<texture> even; // even value texture
};

/**
\brief Noise texture that takes floats between 0 and 1 and creates grey colors.
*/
class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    /*
    \brief Function that creates grey colors from point p. 
    Makes color proportional to a sine function, and use turbulence to adjust the phase (so it shifts x in sin(x)) which makes the stripes undulate. 
    Commenting out straight noise and turbulence, and giving a marble-like effect.

    \param u ray direction x axis
    \param v ray direction y axis
    \param p point p from camera
    */
    virtual color value(double u, double v, const point3& p) const override {
        return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }

public:
    perlin noise; // perlin noise class
    double scale; // scale of noise
};

/**
\brief Texture class that holds an image.

Read in an image and use the 2D (u,v) texture coordinate to index into the image. Texture class that holds an image.
Uses texture coordinates instead of image pixel coordinates. These are just some form of fractional position in the image. 
For example, for pixel (i,j) in an N_x by N_y image, the image texture position is: u = i/(N_x−1) and v = j/(N_y−1).
*/
class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    image_texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        delete data;
    }

    virtual color value(double u, double v, const vec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char* data; // image data
    int width, height; // image characteristics
    int bytes_per_scanline; // bytes per scanline
};

#endif
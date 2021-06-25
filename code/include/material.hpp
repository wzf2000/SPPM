#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "brdf.hpp"
#include "texture.hpp"
#include <iostream>

class Material {
private:
    double clamp(double x) {
        return x > 0 ? x : 0;
    }

public:

    explicit Material(int brdf = DIFFUSE, Texture *t = nullptr, Texture *b = nullptr, const Vector3f &e_color = Vector3f::ZERO) : brdf(brdf), texture(t), bump(b), emission(e_color) {}

    virtual ~Material() = default;

    Texture *texture, *bump;

    int brdf;

    Vector3f emission;

protected:
};


#endif // MATERIAL_H

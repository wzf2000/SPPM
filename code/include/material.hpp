#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "brdf.hpp"
#include "texture.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
private:
    float clamp(float x) {
        return x > 0 ? x : 0;
    }

public:

    explicit Material(int brdf = DIFFUSE, Texture *t = nullptr) : brdf(brdf), texture(t) {}

    virtual ~Material() = default;

    Texture *texture;

    int brdf;

protected:
};


#endif // MATERIAL_H

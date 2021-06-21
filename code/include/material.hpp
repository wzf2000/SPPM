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

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0, int brdf = DIFFUSE, Texture *t = nullptr) :
            diffuseColor(d_color), specularColor(s_color), shininess(s), brdf(brdf), texture(t) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        shaded += diffuseColor * clamp(Vector3f::dot(dirToLight, hit.getNormal()));
        Vector3f reflectLight = 2 * Vector3f::dot(hit.getNormal(), dirToLight) * hit.getNormal() - dirToLight;
        shaded += specularColor * pow(clamp(Vector3f::dot(reflectLight, -ray.getDirection())), shininess);
        shaded = shaded * lightColor;
        return shaded;
    }

    Texture *texture;

    int brdf;

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
};


#endif // MATERIAL_H

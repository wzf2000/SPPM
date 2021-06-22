#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include "object3d.hpp"
#include "math.hpp"
#include "ray.hpp"

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const = 0;

    virtual Ray generateRay() const = 0;
    
    virtual Vector3f getColor() const = 0;
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
    }

    Ray generateRay() const override {
        double alpha = Math::random(0, 2 * M_PI);
        Vector3f s = Vector3f(cos(alpha), 0, sin(alpha));
        return Ray(s, direction);
    }

    Vector3f getColor() const override {
        return color;
    }

private:

    Vector3f direction;
    Vector3f color;

};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vector3f &sourceP, double sourceR, const Vector3f &sourceN, const Vector3f &color) : sourceP(sourceP), sourceR(sourceR), sourceN(sourceN), color(color) {}

    ~PointLight() override = default;

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (sourceP - p);
        dir = dir / dir.length();
        col = color;
    }

    Ray generateRay() const override {
        double alpha = Math::random(0, 2 * M_PI);
        Vector3f s = sourceP +  Vector3f(cos(alpha), 0, sin(alpha)) * sourceR;
        Vector3f d = Math::sampleReflectedRay(sourceN);
        d.normalize();
        return Ray(s + d * Math::eps, d);
    }

    Vector3f getColor() const override {
        return color;
    }

private:

    Vector3f sourceP;
    double sourceR;
    Vector3f sourceN;
    Vector3f color;

};

#endif // LIGHT_H

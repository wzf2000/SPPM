#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() : center(0, 0, 0), radius(0) {
        // unit ball at the center
    }

    Sphere(const Vector3f &center, double radius, Material *material) : Object3D(material), center(center), radius(radius) {
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        Vector3f l = center - r.getOrigin();
        if (l.squaredLength() != radius * radius) {
            bool inside = l.squaredLength() < radius * radius;
            double t_p = Vector3f::dot(l, r.getDirection());
            if (t_p < 0 && !inside) return false;
            double d_square = l.squaredLength() - t_p * t_p;
            if (d_square > radius * radius) return false;
            double t_prime = sqrt(radius * radius - d_square);
            double t = inside ? t_p + t_prime : t_p - t_prime;
            if (t > h.getT() || t < tmin) return false;
            Vector3f n = (r.getOrigin() + t * r.getDirection()) - center;
            n *= inside ? -1 : 1;
            n.normalize();
            h.set(t, this->material, n);
        } else {
            double t_p = Vector3f::dot(l, r.getDirection());
            if (t_p <= 0) return false;
            double t = 2 * t_p;
            Vector3f n = center - (r.getOrigin() + t * r.getDirection());
            n.normalize();
            h.set(t, this->material, n);
        }
        return true;
    }

protected:
    Vector3f center;
    double radius;
};


#endif

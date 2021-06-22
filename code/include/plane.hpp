#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() = delete;

    Plane(const Vector3f &normal, double d, Material *m) : Object3D(m), normal(normal), d(d) {
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        double x = Vector3f::dot(normal, r.getDirection());
        if (x == 0) return false;
        double t = (d + Vector3f::dot(normal, r.getOrigin())) / x;
        if (t < tmin || t > h.getT()) return false;
        if (Vector3f::dot(normal, r.getDirection()) < 0)
            h.set(t, this->material, normal);
        else
            h.set(t, this->material, -normal);
        return true;
    }

protected:
    Vector3f normal;
    double d;

};

#endif //PLANE_H
		


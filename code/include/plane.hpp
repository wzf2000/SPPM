#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

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
        if (x > -1e-6) return false;
        double t = (d - Vector3f::dot(normal, r.getOrigin())) / x;
        if (t < tmin || t > h.getT()) return false;
        Vector3f point = r.pointAtParameter(t);
        double v = point.y();
        double u = Vector3f::dot(point - d * normal, Vector3f::cross(Vector3f::UP, normal));
        if (Vector3f::dot(normal, r.getDirection()) < 0)
            h.set(t, this->material, getNormal(u, v), nullptr, material->texture->getColor(u, v));
        else
            h.set(t, this->material, -getNormal(u, v), nullptr, material->texture->getColor(u, v));
        return true;
    }

    Vector3f getNormal(double u, double v) {
        if (!material->bump) return normal;
        Vector2f grad = Vector2f::ZERO;
        double f = material->bump->getDisturb(u, v, grad);
        if (fabs(f) < DBL_EPSILON) return normal;
        Vector3f uaxis = Vector3f::cross(Vector3f::UP, normal);
        if (uaxis.squaredLength() < DBL_EPSILON) return normal;
        return Vector3f::cross(uaxis + normal * grad[0], Vector3f::UP + normal * grad[1]);
    }

protected:
    Vector3f normal;
    double d;

};

#endif //PLANE_H
		


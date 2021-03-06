#ifndef DISC_H
#define DISC_H

#include "object3d.hpp"
#include "math.hpp"
#include <vecmath.h>
#include <cmath>

class Disc : public Object3D {
public:
    Disc() : center(0, 0, 0), radius(0) {
        // unit disc at the center
    }

    Disc(const Vector3f &center, double radius, Material *material) : Object3D(material), center(center), radius(radius) {
    }

    ~Disc() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        double t = 0;
        if (fabs(r.getDirection().y()) < Math::eps && fabs(r.getOrigin().y() - center.y()) > Math::eps)
            return false;
        t = (center.y() - r.getOrigin().y()) / r.getDirection().y();
        if (t > h.getT() || t < tmin) return false;
        Vector3f p = r.getOrigin() + r.getDirection() * t;
        if (Math::sqr(p.x() - center.x()) + Math::sqr(p.z() - center.z()) <= radius * radius) {
            h.set(t, material, Vector3f(0, -1, 0), nullptr, material->texture->query(r.pointAtParameter(t)));
            return true;
        }
        else return false;
    }

    Ray generateRandomRay() const override {
        double alpha = Math::random(0, 2 * M_PI);
        Vector3f s = center +  Vector3f(cos(alpha), 0, sin(alpha)) * 0.2;
        Vector3f d = Math::sampleReflectedRay(Vector3f(0, -1, 0));
        d.normalize();
        return Ray(s + d * Math::eps, d);
    }

protected:
    Vector3f center;
    double radius;
};


#endif

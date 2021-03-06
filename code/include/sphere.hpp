#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

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
            Vector3f n = r.pointAtParameter(t) - center;
            n.normalize();
            double u = 0.5 + atan2(n.x(), n.z()) / (2 * M_PI);
            double v = 0.5 - asin(n.y()) / M_PI;
            n *= inside ? -1 : 1;
            h.set(t, this->material, getNormal(n, r.pointAtParameter(t) - center, u, v), &center, material->texture->getColor(u, v));
        } else {
            double t_p = Vector3f::dot(l, r.getDirection());
            if (t_p <= 0) return false;
            double t = 2 * t_p;
            Vector3f n = center - r.pointAtParameter(t);
            n.normalize();
            double u = 0.5 + atan2(-n.x(), -n.z()) / (2 * M_PI);
            double v = 0.5 - asin(-n.y()) / M_PI;
            h.set(t, this->material, getNormal(n, r.pointAtParameter(t) - center, u, v), &center, material->texture->getColor(u, v));
        }
        return true;
    }

    Vector3f getNormal(const Vector3f &n, const Vector3f &p, double u, double v) {
        if (!material->bump) return n;
        Vector2f grad = Vector2f::ZERO;
        double f = material->bump->getDisturb(u, v, grad);
        if (fabs(f) < DBL_EPSILON) return n;
        double phi = u * 2 * M_PI, theta = M_PI * v;
        Vector3f pu(-p.z(), 0, p.x());
        Vector3f pv(p.y() * cos(phi), -radius * sin(theta), p.y() * sin(phi));
        if (pu.squaredLength() < DBL_EPSILON) return n;
        return Vector3f::cross(pu + n * grad[0] / (2 * M_PI), pv + n * grad[1] / M_PI).normalized();
    }

    bool intersect_media(const Ray &r, double &t_near, double &t_far) {
        Vector3f l = center - r.getOrigin();
        Vector3f dir = r.getDirection();
        double b = Vector3f::dot(l, dir);
        double det = Math::sqr(b) - l.squaredLength() + Math::sqr(radius);
        if (det < 0) return false;
        else det = sqrt(det);
        t_near = (b - det <= 0) ? 0 : b - det;
        t_far = b + det;
        return b - det > 1e-4 ? true : (b + det > 1e-4 ? true : false);
    }

    Ray generateRandomRay() const override {
        // Sphere Point Picking
        double x = Math::random(-1, 1), y = Math::random(-1, 1);
        double r2 = Math::sqr(x) + Math::sqr(y);
        while (r2 >= 1) {
            x = Math::random(-1, 1), y = Math::random(-1, 1);
            r2 = Math::sqr(x) + Math::sqr(y);
        }
        Vector3f dir(2 * x * sqrt(1 - r2), 2 * y * sqrt(1 - r2), 1 - 2 * r2);
        return Ray(center + radius * dir, dir);
    }

protected:
    Vector3f center;
    double radius;
};


#endif

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include "mesh.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

static inline void ons(const Vector3f &v1, Vector3f &v2, Vector3f &v3) {
    if (std::abs(v1.x()) > std::abs(v1.y())) {
        double inv = 1. / sqrt(Math::sqr(v1.x()) + Math::sqr(v1.z()));
        v2 = Vector3f(-v1.z() * inv, 0, v1.x() * inv);
    }
    else {
        double inv = 1. / sqrt(Math::sqr(v1.y()) + Math::sqr(v1.z()));
        v2 = Vector3f(0, v1.z() * inv, -v1.y() * inv);
    }
    v3 = Vector3f::cross(v1, v2);
}

static inline Vector3f cosineHemisphere(double r2, double t) {
    double r = sqrt(r2);
    double theta = 2 * M_PI * t;
    double x = r * cos(theta);
    double y = r * sin(theta);
    return Vector3f(x, y, sqrt(std::max(double(0), 1 - r2)));
}

static inline Vector3f diffDir(const Vector3f &normal) {
    Vector3f rotX, rotY;
    ons(normal, rotX, rotY);
    return Matrix3f(rotX, rotY, normal) * cosineHemisphere(Math::random(), Math::random());
}

class Triangle: public Object3D {

public:
    Triangle() = delete;

    // a b c are three vertex positions of the triangle
    Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m, Mesh *par = nullptr) : Object3D(m), par(par) {
        normal = Vector3f::cross(a - b, a - c);
        normal.normalize();
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
    }

    Vector3f min() {
        return ::min(vertices[0], ::min(vertices[1], vertices[2]));
    }
    
    Vector3f max() {
        return ::max(vertices[0], ::max(vertices[1], vertices[2]));
    }

    bool intersect( const Ray& ray,  Hit& hit , double tmin) override {
        Vector3f E1 = vertices[0] - vertices[1];
        Vector3f E2 = vertices[0] - vertices[2];
        Vector3f S = vertices[0] - ray.getOrigin();
        double t = Matrix3f(S, E1, E2).determinant() / Matrix3f(ray.getDirection(), E1, E2).determinant();
        double beta = Matrix3f(ray.getDirection(), S, E2).determinant() / Matrix3f(ray.getDirection(), E1, E2).determinant();
        double gamma = Matrix3f(ray.getDirection(), E1, S).determinant() / Matrix3f(ray.getDirection(), E1, E2).determinant();
        if (beta < 0 || beta > 1 || gamma < 0 || gamma > 1 || beta + gamma > 1 || t < tmin || t > hit.getT())
            return false;
        if (Vector3f::dot(normal, ray.getDirection()) < 0)
            hit.set(t, this->material, normal, par->calcCenter());
        else
            hit.set(t, this->material, -normal, par->calcCenter());
        return true;
    }

    double intersectPlane(const Ray &ray) {
        Vector3f E1 = vertices[0] - vertices[1], E2 = vertices[0] - vertices[2], S = vertices[0] - ray.getOrigin();
        double t = Matrix3f(S, E1, E2).determinant();
        double n = Matrix3f(ray.getDirection(), E1, E2).determinant();
        t /= n;
        return t;
    }

    Ray generateRandomRay() const override {
        double r1 = Math::random(), r2 = Math::random();
        if (r1 + r2 > 1) {
            r1 = 1 - r1;
            r2 = 1 - r2;
        }
        return Ray(r1 * vertices[1] + r2 * vertices[2] + (1 - r1 - r2) * vertices[0], diffDir(normal));
    }

    Vector3f normal;
    Vector3f vertices[3];
    Mesh *par;
protected:

};

#endif //TRIANGLE_H

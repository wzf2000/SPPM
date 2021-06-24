#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include "mesh.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

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
            hit.set(t, this->material, normal, par->calcCenter(), material->texture->query(ray.pointAtParameter(t)));
        else
            hit.set(t, this->material, -normal, par->calcCenter(), material->texture->query(ray.pointAtParameter(t)));
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
        return Ray(r1 * vertices[1] + r2 * vertices[2] + (1 - r1 - r2) * vertices[0], Math::sampleReflectedRay(normal));
    }

    Vector3f normal;
    Vector3f vertices[3];
    Mesh *par;
protected:

};

#endif //TRIANGLE_H

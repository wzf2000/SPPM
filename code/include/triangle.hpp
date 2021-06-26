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
        normal = Vector3f::cross(b - a, c - a);
        normal.normalize();
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
    }

    void setVT(const Vector2f &_a, const Vector2f &_b, const Vector2f &_c) {
        at = _a, bt = _b, ct = _c;
        tSet = true;
    }

    void setVNormal(const Vector3f &_a, const Vector3f &_b, const Vector3f &_c) {
        an = _a, bn = _b, cn = _c;
        nSet = true;
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
        Vector3f point = ray.pointAtParameter(t);
        Vector3f va = (vertices[0] - point), vb = (vertices[1] - point), vc = (vertices[2] - point);
        double ra = Vector3f::cross(vb, vc).length();
        double rb = Vector3f::cross(vc, va).length();
        double rc = Vector3f::cross(va, vb).length();
        double u = beta, v = gamma;
        if (tSet) {
            Vector2f uv = (ra * at + rb * bt + rc * ct) / (ra + rb + rc);
            u = uv.x(), v = 1 - uv.y();
        }
        if (Vector3f::dot(normal, ray.getDirection()) < 0)
            hit.set(t, this->material, getNormal(ra, rb, rc), par->calcCenter(), material->texture->getColor(u, v));
        else
            hit.set(t, this->material, -getNormal(ra, rb, rc), par->calcCenter(), material->texture->getColor(u, v));
        return true;
    }

    Vector3f getNormal(double ra, double rb, double rc) {
        if (!nSet) return normal;
        return (ra * an + rb * bn + rc * cn).normalized();
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
    Vector2f at, bt, ct;
    Vector3f an, bn, cn;
    bool nSet = false;
    bool tSet = false;

};

#endif //TRIANGLE_H

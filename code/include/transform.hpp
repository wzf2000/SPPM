#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

class Transform : public Object3D {
public:
    Transform() = delete;

    Transform(const Matrix4f &m, Object3D *obj, Material *material) : Object3D(material), o(obj) {
        transform = m.inverse();
    }

    ~Transform() {
    }

    virtual bool intersect(const Ray &r, Hit &h, double tmin) {
        Vector3f trSource = transformPoint(transform, r.getOrigin());
        Vector3f trDirection = transformDirection(transform, r.getDirection());
        double length = trDirection.length();
        trDirection.normalize();
        Ray tr(trSource, trDirection);
        Hit hh;
        bool inter = o->intersect(tr, hh, tmin);
        if (inter && hh.getT() / length < h.getT()) {
            if (center) delete center;
            center = hh.center ? new Vector3f(transformPoint(transform, *hh.center)) : nullptr;
            h.set(hh.getT() / length, hh.getMaterial(), transformDirection(transform.transposed(), hh.getNormal()).normalized(), center);
            return true;
        }
        return false;
    }

protected:
    Object3D *o; //un-transformed object
    Matrix4f transform;
    Vector3f *center;
};

#endif //TRANSFORM_H

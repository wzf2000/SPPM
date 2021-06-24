#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"
#include "material.hpp"

class Mesh;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        t = 1e100;
    }

    Hit(double _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
    }

    // destructor
    ~Hit() = default;

    double getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    void set(double _t, Material *m, const Vector3f &n, Vector3f *c = nullptr, Vector3f col = Vector3f::ZERO) {
        t = _t;
        material = m;
        normal = n;
        center = c;
        color = col;
    }
    Vector3f *center = nullptr;
    Vector3f color;

private:
    double t;
    Material *material;
    Vector3f normal;

};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H

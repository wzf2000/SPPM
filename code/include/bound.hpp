#pragma once

#include <vecmath.h>
#include <vector>
#include "ray.hpp"

class Bound {
    Vector3f bounds[2];
    Vector3f *center;

public:
    Bound() {
        bounds[0] = Vector3f(1e100);
        bounds[1] = Vector3f(-1e100);
        center = new Vector3f((bounds[0] + bounds[1]) / 2);
    }

    Bound(const Vector3f &mn, const Vector3f &mx) {
        bounds[0] = mn;
        bounds[1] = mx;
        center = new Vector3f((bounds[0] + bounds[1]) / 2);
    }

    void set(const Vector3f &mn, const Vector3f &mx) {
        bounds[0] = mn;
        bounds[1] = mx;
        *center = Vector3f((bounds[0] + bounds[1]) / 2);
    }

    Vector3f *getCenter() const {
        return center;
    }

    void updateBound(const Vector3f &v) {
        bounds[0] = min(bounds[0], v);
        bounds[1] = max(bounds[1], v);
        *center = Vector3f((bounds[0] + bounds[1]) / 2);
    }

    bool intersect(const Ray &r, double &t_min) {
        Vector3f o(r.getOrigin()), invdir(1 / r.getDirection());
        std::vector<int> sign = {invdir.x() < 0, invdir.y() < 0, invdir.z() < 0};
        t_min = 1e100;
        double tmin, tmax, tymin, tymax, tzmin, tzmax;
        tmin = (bounds[sign[0]].x() - o.x()) * invdir.x();
        tmax = (bounds[1 - sign[0]].x() - o.x()) * invdir.x();
        tymin = (bounds[sign[1]].y() - o.y()) * invdir.y();
        tymax = (bounds[1 - sign[1]].y() - o.y()) * invdir.y();
        if ((tmin > tymax) || (tmax < tymin)) return false;
        tmin = std::max(tymin, tmin);
        tmax = std::min(tymax, tmax);
        tzmin = (bounds[sign[2]].z() - o.z()) * invdir.z();
        tzmax = (bounds[1 - sign[2]].z() - o.z()) * invdir.z();
        if ((tmin > tzmax) || (tmax < tzmin)) return false;
        tmin = std::max(tzmin, tmin);
        tmax = std::min(tzmax, tmax);
        t_min = tmin;
        return true;
    }
};

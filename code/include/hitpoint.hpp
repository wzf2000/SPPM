#pragma once
#include <vecmath.h>

#include "hit.hpp"
#include "brdf.hpp"

class HitPoint {
public:
    Vector3f p;
    Vector3f weight, flux, fluxLight;
    Vector3f dir, norm;
    int n;
    BRDF brdf;
    double r2;
    bool valid;
    HitPoint() : flux(0), fluxLight(0), r2(1e-5), n(0), valid(false) {}
};
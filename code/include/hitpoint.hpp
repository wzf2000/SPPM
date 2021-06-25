#pragma once
#include <vecmath.h>

#include "hit.hpp"
#include "brdf.hpp"

#define INIT_R2 1e-5

class HitPoint {
public:
    Vector3f p;
    Vector3f weight, flux, fluxLight;
    Vector3f dir, norm;
    int n;
    BRDF brdf;
    double r2;
    HitPoint() : flux(0), fluxLight(0), r2(INIT_R2), n(0) {}
};
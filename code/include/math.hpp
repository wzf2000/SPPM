#pragma once
#include <iostream>
#include <random>
#include <vecmath.h>

namespace Math {

    const double alpha = 0.7;
    const double eps = 1e-6;
    const float gamma = 0.5;

    double sqr(double x);

    double random(double l = 0, double r = 1);

    Vector3f sampleReflectedRay(Vector3f norm, double s = 1);

    static inline uint32_t hash(uint32_t x);
    static inline uint64_t hash64(uint64_t x);

}

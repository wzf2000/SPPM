#pragma once
#include "ray.hpp"

class BRDF {
public:
    double specular, diffuse, refraction;
    double rho_d, rho_s, phong_s;
    double refractiveIndex;
    BRDF() {}
    BRDF(double specular, double diffuse, double refraction, double rho_d, double rho_s, double phong_s, double refractiveIndex) : specular(specular), diffuse(diffuse), refraction(refraction), rho_d(rho_d), rho_s(rho_s), phong_s(phong_s), refractiveIndex(refractiveIndex) {}
};

enum { 
    DIFFUSE, MIRROR, GLASS, LIGHT, MARBLE, FLOOR, WALL, DESK, STANFORD_MODEL, WATER, TEAPOT 
} BRDF_TYPES;

const BRDF BRDFs[] = {
    BRDF(0, 1, 0,       0.7, 0, 0,      0), // DIFFUSE
    BRDF(1, 0, 0,       0, 0, 0,        0), // MIRROR
    BRDF(0, 0, 1,       0, 0, 0,        1.65), // GLASS
    BRDF(0, 1, 0,       0, 0, 0,        0), // LIGHT
    BRDF(0.1, 0.9, 0,   1, 0, 50,       0 ), // MARBLE
    BRDF(0.1, 0.9, 0,   0.9, 0.1, 50,   0), // FLOOR
    BRDF(0, 1, 0,       1, 0, 0,        0), // WALL
    BRDF(0, 1, 0,       1, 0, 0,        0), // DESK
    BRDF(0, 1, 0,       0.9, 0.1, 10,   1), // STANFORD_MODEL
    BRDF(0, 0, 1,       0, 0, 0,        1.33), // WATER
    BRDF(0, 0, 1,       0, 0, 0,        1.5), // TEAPOT
};

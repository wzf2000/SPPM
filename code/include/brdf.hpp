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
    BRDF(0, 1, 0,        0.7, 0, 0,      0), // DIFFUSE 0
    BRDF(1, 0, 0,        0, 0, 0,        0), // MIRROR 1
    BRDF(0, 0, 1,        0, 0, 0,        1.65), // GLASS 2
    BRDF(0, 1, 0,        0, 0, 0,        0), // LIGHT 3
    BRDF(0.1, 0.9, 0,    1, 0, 50,       0 ), // MARBLE 4
    BRDF(0.1, 0.9, 0,    0.9, 0.1, 50,   0), // FLOOR 5
    BRDF(0, 1, 0,        1, 0, 0,        0), // WALL 6
    BRDF(0, 1, 0,        1, 0, 0,        0), // DESK 7
    BRDF(0, 1, 0,        0.9, 0.1, 10,   1), // STANFORD_MODEL 8
    BRDF(0, 0, 1,        0, 0, 0,        1.33), // WATER 9
    BRDF(0, 0, 1,        0, 0, 0,        1.5), // TEAPOT 10
    BRDF(0.696, 0.261, 0.043, 1, 0, 0,   1.5), // 11
    BRDF(0, 0, 1,        0, 0, 0,        1.8), // 12
    BRDF(0.01, 0.99, 0,  1, 0, 0,        0), // 13
    BRDF(0.2, 0.8, 0,    1, 0, 0,        0), // 14
    BRDF(0.7, 0.3, 0,    1, 0, 0,        0), // 15
};

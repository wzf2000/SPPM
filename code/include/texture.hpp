#pragma once
#include <vecmath.h>
#include <iostream>
#include <cstring>
#include "image.hpp"

class Texture {
    Image *image;
    Vector3f color;
    Vector3f x, y;
    double xb, yb;

public:
    Texture(char *filename, Vector3f x, double xb, Vector3f y, double yb);

    Texture(Vector3f color);

    Vector3f query(double x, double y);

    Vector3f query(const Vector3f &p);

    Vector3f getColor(double u, double v);
};

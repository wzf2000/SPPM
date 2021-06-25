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

    Texture(char *filename);

    Texture(Vector3f color);

    Vector3f query(double x, double y) const;

    Vector3f query(const Vector3f &p) const;

    Vector3f getColor(double u, double v) const;

    double getGray(double x, double y) const;

    double getDisturb(double u, double v, Vector2f &grad) const;
};

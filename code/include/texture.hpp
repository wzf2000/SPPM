#pragma once
#include <vecmath.h>
#include <iostream>
#include "image.hpp"

class Texture {
    Image *image;
    Vector3f color;
    Vector3f x, y;
    double xb, yb;

public:
    Texture(char *filename, Vector3f x, double xb, Vector3f y, double yb) {
        FILE *file = fopen(filename, "r");
        int width, height;
        fscanf(file, "%*s%d%d%*d", &width, &height);
        image = new Image(width, height);
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int r, g, b;
                fscanf(file, "%d%d%d", &r, &g, &b);
                image->SetPixel(j, i, Vector3f(r / 255., g / 255., b / 255.));
            }
        }
        fclose(file);
        this->x = x;
        this->xb = xb;
        this->y = y;
        this->yb = yb;
    }

    Texture(Vector3f color) {
        this->image = nullptr;
        this->color = color;
    }

    Vector3f query(double x, double y) {
        int _x = x * image->Height();
        int _y = y * image->Width();
        if (_x >= 0 && _x < image->Height() && _y >= 0 && _y < image->Width()) 
            return image->GetPixel(_x, _y);
        else
            return Vector3f::ZERO;
    }

    Vector3f query(Vector3f p) {
        if (!image) return color;
        double x = Vector3f::dot(x, p) + xb;
        double y = Vector3f::dot(y, p) + yb;
        return query(x, y);
    }
};

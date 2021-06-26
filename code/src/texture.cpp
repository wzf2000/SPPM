#define STB_IMAGE_IMPLEMENTATION
#include "texture.hpp"
#include "stb_image.h"

Texture::Texture(char *filename, Vector3f x, double xb, Vector3f y, double yb) {
    name = filename;
    int len = strlen(filename);
    int c;
    int width, height;
    if (strcmp(".ppm", filename + len - 4) == 0) {
        FILE *file = fopen(filename, "r");
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
    }
    else {
        unsigned char *pic = stbi_load(filename, &width, &height, &c, 0);
        image = new Image(width, height);
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int index = i * width * c + j * c;
                int r = pic[index], g = pic[index + 1], b = pic[index + 2];
                image->SetPixel(j, i, Vector3f(r / 255., g / 255., b / 255.));
            }
        }
    }
    this->x = x;
    this->xb = xb;
    this->y = y;
    this->yb = yb;
    std::cerr << filename << " width = " << width << " height = " << height << " color = " << c << std::endl;
}

Texture::Texture(char *filename) {
    name = filename;
    int c;
    int width, height;
    unsigned char *pic = stbi_load(filename, &width, &height, &c, 0);
    image = new Image(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int index = i * width * c + j * c;
            int r = pic[index], g = pic[index] + 1, b = pic[index + 2];
            image->SetPixel(j, i, Vector3f(r / 255., g / 255., b / 255.));
        }
    }
    std::cerr << filename << " width = " << width << " height = " << height << " color = " << c << std::endl;
}

Texture::Texture(Vector3f color) {
    this->image = nullptr;
    this->color = color;
}

Vector3f Texture::query(double x, double y) const {
    int _x = x * image->Height();
    int _y = y * image->Width();
    if (_x >= 0 && _x < image->Height() && _y >= 0 && _y < image->Width()) 
        return image->GetPixel(_y, _x);
    else
        return Vector3f::ZERO;
}

Vector3f Texture::query(const Vector3f &p) const {
    if (!image) return color;
    double xx = Vector3f::dot(x, p) + xb;
    double yy = Vector3f::dot(y, p) + yb;
    return query(xx, yy);
}

Vector3f Texture::getColor(double u, double v) const {
    if (!image) return color;
    u -= int(u);
    v -= int(v);
    if (u < 0) u += 1;
    if (v < 0) v += 1;
    u = u * image->Width();
    v = v * image->Height();
    int x = u, y = v;
    double alpha = u - x, beta = v - y;
    Vector3f ret = Vector3f::ZERO;
    ret += (1 - alpha) * (1 - beta) * image->GetPixel(x, y);
    ret += alpha * (1 - beta) * image->GetPixel(x + 1, y);
    ret += (1 - alpha) * beta * image->GetPixel(x, y + 1);
    ret += alpha * beta * image->GetPixel(x + 1, y + 1);
    return ret;
}

double Texture::getDisturb(double u, double v, Vector2f &grad) const {
    if (!image) return 0;
    double disturb = getGray(u, v);
    double du = 1.0 / image->Width(), dv = 1.0 / image->Height();
    grad[0] = image->Width() * (getGray(u + du, v) - getGray(u - du, v)) / 2.0;
    grad[1] = image->Height() * (getGray(u, v + dv) - getGray(u, v - dv)) / 2.0;
    return disturb;
}

double Texture::getGray(double x, double y) const {
    return (Vector3f::dot(getColor(x, y), Vector3f(1. / 3.)) - 0.5) * 2;
}

#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "math.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
		this->horizontal.normalize();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    friend class Renderer;

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, double angle1, double angle2) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fy = (double) height / (2 * tan(angle1 / 2));
        fx = (double) width / (2 * tan(angle2 / 2));
        cx = width / 2.0f;
        cy = height / 2.0f;
    }

    Ray generateRay(const Vector2f &point) override {
        double csx = (point.x() - cx) / fx;
        double csy = (point.y() - cy) / fy;
        Vector3f dir(csx, -csy, 1.0f);
        Matrix3f R(this->horizontal, -this->up, this->direction);
        dir = R * dir;
        dir = dir / dir.length();
        Ray ray(this->center, dir);
        return ray;
    }

    friend class Renderer;

private:
    double fx, fy;
    double cx, cy;

};

class FixedCamera : public Camera {
public:
    FixedCamera(const Vector3f &center, double fx, double fy, double aperture, double focus, int imgW, int imgH) : Camera(center, Vector3f::UP, Vector3f::FORWARD, imgW, imgH), fx(fx), fy(fy), aperture(aperture), focus(focus) {}

    Ray generateRay(const Vector2f &point) override {
        int cx = width / 2, cy = height / 2;
        double csx = (point.x() - cx) / fx / width;
        double csy = (point.y() - cy) / fy / height;
        Vector3f p(csx, csy, 0);
        Vector3f dir = p - center;
        return Ray(center, dir);
    }

    friend class Renderer;

private:
    double fx, fy;
    double aperture, focus;
};

#endif //CAMERA_H

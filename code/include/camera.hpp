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
    virtual Ray generateRay() = 0;
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

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fx = fy = (float) height / (2 * tanf(angle / 2));
        cx = width / 2.0f;
        cy = height / 2.0f;
    }

    Ray generateRay(const Vector2f &point) override {
        float csx = (point.x() - cx) / fx;
        float csy = (point.y() - cy) / fy;
        Vector3f dir(csx, -csy, 1.0f);
        Matrix3f R(this->horizontal, -this->up, this->direction);
        dir = R * dir;
        dir = dir / dir.length();
        Ray ray(this->center, dir);
        return ray;
    }

    Ray generateRay() override {
        double alpha = Math::random(0, 2 * M_PI);
        Vector3f s = center +  Vector3f(cos(alpha), 0, sin(alpha)) * 0.2;
        Vector3f d = sampleReflectedRay(0.2);
        d.normalize();
        return Ray(s + d * 1e-6, d);
    }

    friend class Renderer;

private:
    float fx, fy;
    float cx, cy;

    Vector3f sampleReflectedRay(Vector3f norm, double s = 1) {
        Vector3f u = Vector3f::cross(Vector3f(1, 0, 0), norm);
        if (u.squaredLength() < 1e-6) u = Vector3f::cross(Vector3f(0, 1, 0), norm);
        u.normalize();
        Vector3f v = Vector3f::cross(norm, u);
        v.normalize();
        double theta = Math::random(0, 2 * M_PI);
        double phi = asin(pow(Math::random(0, 1), 1. / (s + 1)));
        return (norm * cos(phi) + (u * cos(theta) + v * sin(theta)) * sin(phi)).normalized();
    }

};

#endif //CAMERA_H

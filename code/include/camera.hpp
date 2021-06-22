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
            const Vector3f &up, int imgW, int imgH, double angle1, double angle2) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fy = (double) height / (2 * tanf32x(angle1 / 2));
        fx = (double) width / (2 * tanf32x(angle2 / 2));
        std::cerr << fx / width << " " << fy / height << std::endl;
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

    Ray generateRay() override {
        double alpha = Math::random(0, 2 * M_PI);
        Vector3f s = center +  Vector3f(cos(alpha), 0, sin(alpha)) * 0.2;
        Vector3f d = Math::sampleReflectedRay(0.2);
        d.normalize();
        return Ray(s + d * Math::eps, d);
    }

    friend class Renderer;

private:
    double fx, fy;
    double cx, cy;

};

#endif //CAMERA_H

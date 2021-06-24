#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include "bound.hpp"
#include "renderer.hpp"
#include <tuple>

class RevSurface : public Object3D {

    // Definition for drawable surface.
    // typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;
    Curve *pCurve;
    Bound bound;

    // struct Surface {
    //     std::vector<Vector3f> VV;
    //     std::vector<Vector3f> VN;
    //     std::vector<Tup3u> VF;
    // } surface;

public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // surface.VV.clear();
        // surface.VN.clear();
        // surface.VF.clear();
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        // Surface is just a struct that contains vertices, normals, and
        // faces.  VV[i] is the position of vertex i, and VN[i] is the normal
        // of vertex i.  A face is a triple i,j,k corresponding to a triangle
        // with (vertex i, normal i), (vertex j, normal j), ...
        // Currently this struct is computed every time when canvas refreshes.
        // You can store this as member function to accelerate rendering.

        // std::vector<CurvePoint> curvePoints;
        // pCurve->discretize(5, curvePoints);
        // const int steps = 40;
        // for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
        //     const CurvePoint &cp = curvePoints[ci];
        //     for (unsigned int i = 0; i < steps; ++i) {
        //         double t = (double) i / steps;
        //         Quat4f rot;
        //         rot.setAxisAngle(t * 2 * 3.14159, Vector3f::UP);
        //         Vector3f pnew = Matrix3f::rotation(rot) * cp.V;
        //         Vector3f pNormal = Vector3f::cross(cp.T, -Vector3f::FORWARD);
        //         Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
        //         surface.VV.push_back(pnew);
        //         surface.VN.push_back(nnew);
        //         int i1 = (i + 1 == steps) ? 0 : i + 1;
        //         if (ci != curvePoints.size() - 1) {
        //             surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
        //             surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
        //         }
        //     }
        // }
        bound.set(Vector3f(-pCurve->radius, pCurve->ymin - 3, -pCurve->radius),
                  Vector3f(pCurve->radius, pCurve->ymax + 3, pCurve->radius));
    }

    ~RevSurface() override {
        delete pCurve;
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // bool result = false;
        // for (auto face: surface.VF) {
        //     Triangle triangle(surface.VV[std::get<0>(face)], surface.VV[std::get<1>(face)], surface.VV[std::get<2>(face)], material);
        //     result |= triangle.intersect(r, h, tmin);
        // }
        // if (result) {
        //     Vector3f point = r.pointAtParameter(h.getT());
        //     double disSquare = (point - r.getOrigin()).squaredLength() - Vector3f::dot(point - r.getOrigin(), r.getDirection()) * Vector3f::dot(point - r.getOrigin(), r.getDirection());
        // }
        // return result;
        double t, theta, mu;
        if (!bound.intersect(r, t) || t > h.getT()) return false;
        getUV(r, t, theta, mu);
        Vector3f normal, point;
        if (!newton(r, t, theta, mu, normal, point)) return false;
        ++Renderer::count;
        if (!isnormal(mu) || !isnormal(theta) || !isnormal(t)) return false;
        if (t < tmin || t > h.getT() || mu < pCurve->range[0] || mu > pCurve->range[1]) return false;
        h.set(t, material, normal.normalized(), bound.getCenter());
        return true;
    }

    Vector3f getPoint(const double &theta, const double &mu, Vector3f &dtheta, Vector3f &dmu) {
        Vector3f point;
        Quat4f rot;
        rot.setAxisAngle(theta, Vector3f::UP);
        Matrix3f rotMat = Matrix3f::rotation(rot);
        CurvePoint cp = pCurve->getVT(mu);
        point = rotMat * cp.V;
        dmu = rotMat * cp.T;
        dtheta = Vector3f(-cp.V.x() * sin(theta), 0, -cp.V.x() * cos(theta));
        return point;
    }

    bool newton(const Ray &r, double &t, double &theta, double &mu, Vector3f &normal, Vector3f &point) {
        Vector3f dmu, dtheta;
        for (int i = 0; i < 20; ++i) {
            if (theta < 0.0) theta += 2 * M_PI;
            if (theta >= 2 * M_PI) theta = fmod(theta, 2 * M_PI);
            if (mu >= 1) mu = 1.0 - DBL_EPSILON;
            if (mu <= 0) mu = DBL_EPSILON;
            point = getPoint(theta, mu, dtheta, dmu);
            Vector3f f = r.pointAtParameter(t) - point;
            double disSquare = f.squaredLength();
            normal = Vector3f::cross(dmu, dtheta);
            if (disSquare < 1e-4) return true;
            float D = Vector3f::dot(r.getDirection(), normal);
            t -= Vector3f::dot(dmu, Vector3f::cross(dtheta, f)) / D;
            mu -= Vector3f::dot(r.getDirection(), Vector3f::cross(dtheta, f)) / D;
            theta += Vector3f::dot(r.getDirection(), Vector3f::cross(dmu, f)) / D;
        }
        return false;
    }

    void getUV(const Ray &r, const double &t, double &theta, double &mu) {
        Vector3f point(r.pointAtParameter(t));
        theta = atan2(-point.z(), point.x()) + M_PI;
        mu = (pCurve->ymax - point.y()) / (pCurve->ymax - pCurve->ymin);
    }
};

#endif //REVSURFACE_HPP

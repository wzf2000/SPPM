#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include "transform.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual CurvePoint getVT(double t) = 0;
    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
};

class BezierCurve : public Curve {

    double **C;
    int n, k;

    double Bezier(int i, int n, double t) {
        return C[n][i] * pow(1 - t, n - i) * pow(t, i);
    }

    double BezierPrime(int i, int n, double t) {
        return n * (Bezier(i - 1, n - 1, t) - Bezier(i, n - 1, t));
    }

public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        n = controls.size() - 1, k = n;
        C = new double*[n + 1];
        for (int i = 0; i <= n; ++i) {
            C[i] = new double[n + 1];
            if (i) {
                for (int j = 1; j <= n; ++j)
                    C[i][j] = C[i - 1][j] + C[i - 1][j - 1];
                C[i][0] = C[i - 1][0];
            }
            else {
                for (int j = 1; j <= n; ++j)
                    C[i][j] = 0;
                C[i][0] = 1;
            }
        }
    }

    ~BezierCurve() override {
        for (int i = 0; i <= n; ++i)
            delete [] C[i];
        delete [] C;
    }

    CurvePoint getVT(double t) override {
        Vector3f V = Vector3f::ZERO;
        Vector3f T = Vector3f::ZERO;
        for (int i = 0; i <= n; ++i) {
            auto P = controls[i];
            V += Bezier(i, k, t) * P;
            T += BezierPrime(i, k, t) * P;
        }
        T.normalize();
        return (CurvePoint){V, T};
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        for (int ti = 0; ti <= (n + 1) * resolution; ++ti) {
            double t = 1. * ti / resolution / (n + 1);
            data.push_back(getVT(t));
        }
    }

protected:

};

class BsplineCurve : public Curve {

    double *B, *dB;

    int n, k;

    double tpad(int i) {
        return 1. * i / (n + k + 1);
    }

public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        n = controls.size() - 1, k = 3;
        B = new double[n + k + 1];
        dB = new double[n + k + 1];
    }
    
    ~BsplineCurve() override {
        delete [] B;
        delete [] dB;
    }

    CurvePoint getVT(double t) override {
        for (int i = 0; i <= n + k; ++i) {
            B[i] = (tpad(i) <= t && t < tpad(i + 1));
        }
        for (int p = 1; p <= k; ++p) {
            for (int i = 0; i <= n + k - p; ++i) {
                if (p == k) {
                    dB[i] = p * (B[i] / (tpad(i + p) - tpad(i)) - B[i + 1] / (tpad(i + p + 1) - tpad(i + 1)));
                }
                B[i] = (t - tpad(i)) / (tpad(i + p) - tpad(i)) * B[i] + (tpad(i + p + 1) - t) / (tpad(i + p + 1) - tpad(i + 1)) * B[i + 1];
            }
        }
        Vector3f V = Vector3f::ZERO;
        Vector3f T = Vector3f::ZERO;
        for (int i = 0; i <= n; ++i) {
            auto P = controls[i];
            V += B[i] * P;
            T += dB[i] * P;
        }
        T.normalize();
        return (CurvePoint){V, T};
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        for (int ti = k * resolution; ti <= (n + 1) * resolution; ++ti) {
            double t = 1. * ti / resolution / (n + k + 1);
            data.push_back(getVT(t));
        }
    }

protected:

};

#endif // CURVE_HPP

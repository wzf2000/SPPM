#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include "objectkdtree.hpp"

class Triangle;


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m, Vector3f *center = nullptr);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    std::vector<Triangle*> faces;
    ObjectKDTree *kdtree;
    Vector3f *center = nullptr;
    Vector3f *calcCenter();
    bool intersect(const Ray &r, Hit &h, double tmin) override;

    Ray generateRandomRay() const override;

private:

    // Normal can be used for light estimation
    void computeNormal();
};

#endif

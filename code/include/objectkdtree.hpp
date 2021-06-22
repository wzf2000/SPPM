#include <vector>
#include <map>
#include <iostream>
#include "triangle.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "math.hpp"

struct ObjectKDTreeNode {
    Vector3f minCoord, maxCoord;
    vector<Triangle*> faces;
    ObjectKDTreeNode *ls, *rs;
    int l, r;
    bool inside(Triangle *);
};

class ObjectKDTree {
    int n;
    Vector3f *vertexes;
    ObjectKDTreeNode *build(int depth, int d, vector<Triangle*> &faces, Vector3f minCoord, Vector3f maxCoord);
    void getFaces(ObjectKDTreeNode *p, vector<Triangle*> &faces);
public:
    ObjectKDTreeNode *root;
    vector<Triangle*> faces;
    ObjectKDTree(vector<Triangle*> faces);
    double getCuboidIntersection(ObjectKDTreeNode *p, const Ray &ray);
    bool intersect(ObjectKDTreeNode *p, const Ray &ray, Hit &hit, double tmin);
};

const int max_faces = 8;
const int max_depth = 24;

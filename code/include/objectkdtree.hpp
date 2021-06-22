#pragma once

#include <vector>
#include <map>
#include <iostream>
#include "ray.hpp"
#include "hit.hpp"
#include "math.hpp"

using namespace std;

class Triangle;

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
public:
    ObjectKDTreeNode *root;
    ObjectKDTree(vector<Triangle*> faces);
    double getCuboidIntersection(ObjectKDTreeNode *p, const Ray &ray);
    bool intersect(ObjectKDTreeNode *p, const Ray &ray, Hit &hit, double tmin);
};

const int max_faces = 8;
const int max_depth = 24;

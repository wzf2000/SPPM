#include "objectkdtree.hpp"
#include "triangle.hpp"

bool ObjectKDTreeNode::inside(Triangle *face) {
    Vector3f faceMin = face->min();
    Vector3f faceMax = face->max();
    return (faceMin.x() < maxCoord.x() || faceMin.x() == maxCoord.x() && faceMin.x() == faceMax.x())
        && (faceMax.x() > minCoord.x() || faceMax.x() == minCoord.x() && faceMin.x() == faceMax.x())
        && (faceMin.y() < maxCoord.y() || faceMin.y() == maxCoord.y() && faceMin.y() == faceMax.y())
        && (faceMax.y() > minCoord.y() || faceMax.y() == minCoord.y() && faceMin.y() == faceMax.y())
        && (faceMin.z() < maxCoord.z() || faceMin.z() == maxCoord.z() && faceMin.z() == faceMax.z())
        && (faceMax.z() > minCoord.z() || faceMax.z() == minCoord.z() && faceMin.z() == faceMax.z());
}

ObjectKDTreeNode *ObjectKDTree::build(int depth, int d, vector<Triangle*> &faces, Vector3f minCoord, Vector3f maxCoord) {
    ObjectKDTreeNode *p = new ObjectKDTreeNode;
    p->minCoord = minCoord, p->maxCoord = maxCoord;
    Vector3f maxL, minR;
    if (d == 0) {
        maxL = Vector3f((p->minCoord.x() + p->maxCoord.x()) / 2, p->maxCoord.y(), p->maxCoord.z());
        minR = Vector3f((p->minCoord.x() + p->maxCoord.x()) / 2, p->minCoord.y(), p->minCoord.z());
    }
    else if (d == 1) {
        maxL = Vector3f(p->maxCoord.x(), (p->minCoord.y() + p->maxCoord.y()) / 2, p->maxCoord.z());
        minR = Vector3f(p->minCoord.x(), (p->minCoord.y() + p->maxCoord.y()) / 2, p->minCoord.z());
    }
    else {
        maxL = Vector3f(p->maxCoord.x(), p->maxCoord.y(), (p->minCoord.z() + p->maxCoord.z()) / 2);
        minR = Vector3f(p->minCoord.x(), p->minCoord.y(), (p->minCoord.z() + p->maxCoord.z()) / 2);
    }
    p->faces.clear();
    for (auto face : faces)
        if (p->inside(face))
            p->faces.emplace_back(face);
    if (p->faces.size() > max_faces && depth < max_depth) {
        p->ls = build(depth + 1, (d + 1) % 3, p->faces, minCoord, maxL);
        p->rs = build(depth + 1, (d + 1) % 3, p->faces, minR, maxCoord);
        
        vector<Triangle*> faceL = p->ls->faces, faceR = p->rs->faces;
        std::map<Triangle*, int> cnt;
        for (auto face : faceL) cnt[face]++;
        for (auto face : faceR) cnt[face]++;
        p->ls->faces.clear();
        p->rs->faces.clear();
        p->faces.clear();
        for (auto face : faceL)
            if (cnt[face] == 1)
                p->ls->faces.emplace_back(face);
            else
                p->faces.emplace_back(face);
        for (auto face : faceR)
            if (cnt[face] == 1)
                p->rs->faces.emplace_back(face);
    }
    else
        p->ls = p->rs = nullptr;
    return p;
}

ObjectKDTree::ObjectKDTree(vector<Triangle*> faces) {
    Vector3f minCoord = Vector3f(1e100);
    Vector3f maxCoord = Vector3f(-1e100);        
    for (auto face : faces) {
        minCoord = ::min(minCoord, face->min());
        maxCoord = ::max(maxCoord, face->max());
    }
    root = build(1, 0, faces, minCoord, maxCoord);
}

double ObjectKDTree::getCuboidIntersection(ObjectKDTreeNode *p, const Ray &ray) {
    if (!(ray.getOrigin() >= p->minCoord && ray.getOrigin() <= p->maxCoord)) { // outside
        double t = -1e100;
        if (fabs(ray.getDirection().x()) > 0)
            t = std::max<double>(t, std::min((p->minCoord.x() - ray.getOrigin().x()) / ray.getDirection().x(), (p->maxCoord.x() - ray.getOrigin().x()) / ray.getDirection().x()));
        if (fabs(ray.getDirection().y()) > 0)
            t = std::max<double>(t, std::min((p->minCoord.y() - ray.getOrigin().y()) / ray.getDirection().y(), (p->maxCoord.y() - ray.getOrigin().y()) / ray.getDirection().y()));
        if (fabs(ray.getDirection().z()) > 0)
            t = std::max<double>(t, std::min((p->minCoord.z() - ray.getOrigin().z()) / ray.getDirection().z(), (p->maxCoord.z() - ray.getOrigin().z()) / ray.getDirection().z()));
        if (t < -Math::eps) return 1e100;
        Vector3f pp = ray.getOrigin() + ray.getDirection() * t;
        if (!(pp >= p->minCoord && pp <= p->maxCoord)) return 1e100;
        return t;
    }
    else return -1e100;
}

bool ObjectKDTree::intersect(ObjectKDTreeNode *p, const Ray &ray, Hit &hit, double tmin) {
    bool flag = false;
    for (auto face : p->faces) {
        flag |= face->intersect(ray, hit, tmin);
    }
    
    double tl = p->ls ? getCuboidIntersection(p->ls, ray) : 1e100;
    double tr = p->rs ? getCuboidIntersection(p->rs, ray) : 1e100;
    if (tl < tr) {
        if (hit.getT() <= tl) return flag;
        if (p->ls) flag |= intersect(p->ls, ray, hit, tmin);
        if (hit.getT() <= tr) return flag;
        if (p->rs) flag |= intersect(p->rs, ray, hit, tmin);
    }
    else {
        if (hit.getT() <= tr) return flag;
        if (p->rs) flag |= intersect(p->rs, ray, hit, tmin);
        if (hit.getT() <= tl) return flag;
        if (p->ls) flag |= intersect(p->ls, ray, hit, tmin);            
    }
    return flag;
}

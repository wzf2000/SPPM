#include "kdtree.hpp"
#include <algorithm>
#include <iostream>

KDTree::KDTree(std::vector<HitPoint*> hitpoints) {
    n = hitpoints.size();
    this->hitpoints = new HitPoint*[n];
    for (int i = 0; i < n; ++i)
        this->hitpoints[i] = hitpoints[i];
    root = build(0, n - 1, 0);
}

static bool cmpX(HitPoint *a, HitPoint *b) {
    return a->p.x() < b->p.x();
}

static bool cmpY(HitPoint *a, HitPoint *b) {
    return a->p.y() < b->p.y();
}

static bool cmpZ(HitPoint *a, HitPoint *b) {
    return a->p.z() < b->p.z();
}

KDTreeNode* KDTree::build(int l, int r, int d) {
    KDTreeNode *p = new KDTreeNode;
    p->minCoord = Vector3f(1e100, 1e100, 1e100);
    p->maxCoord = Vector3f(-1e100, -1e100, -1e100);
    p->maxr2 = 0;
    for (int i = l; i <= r; ++i) {
        p->minCoord = min(p->minCoord, hitpoints[i]->p);
        p->maxCoord = max(p->maxCoord, hitpoints[i]->p);
        p->maxr2 = std::max(p->maxr2, hitpoints[i]->r2);
    }
    int m = (l + r) >> 1;
    if (d == 0) 
        std::nth_element(hitpoints + l, hitpoints + m, hitpoints + r + 1, cmpX);
    else if (d == 1) 
        std::nth_element(hitpoints + l, hitpoints + m, hitpoints + r + 1, cmpY);
    else 
        std::nth_element(hitpoints + l, hitpoints + m, hitpoints + r + 1, cmpZ);
    p->hitpoint = hitpoints[m];
    p->ls = (l <= m - 1) ? build(l, m - 1, (d + 1) % 3) : nullptr;
    p->rs = (m + 1 <= r) ? build(m + 1, r, (d + 1) % 3) : nullptr;
    return p;
}

void KDTree::del(KDTreeNode *p) {
    if (p->ls) del(p->ls);
    if (p->rs) del(p->rs);
    delete p;
}

void KDTree::update(KDTreeNode *p, Vector3f photon, Vector3f weight, Vector3f d) {
    if (!p) return;
    double mind = 0, maxd = 0;
    if (photon.x() > p->maxCoord.x()) mind += Math::sqr(photon.x()- p->maxCoord.x());
    if (photon.x() < p->minCoord.x()) mind += Math::sqr(p->minCoord.x()- photon.x());
    if (photon.y() > p->maxCoord.y()) mind += Math::sqr(photon.y()- p->maxCoord.y());
    if (photon.y()< p->minCoord.y()) mind += Math::sqr(p->minCoord.y()- photon.y());
    if (photon.z()> p->maxCoord.z()) mind += Math::sqr(photon.z()- p->maxCoord.z());
    if (photon.z()< p->minCoord.z()) mind += Math::sqr(p->minCoord.z()- photon.z());
    if (mind > p->maxr2) return;
    if (p->hitpoint->valid && (photon - p->hitpoint->p).squaredLength() <= p->hitpoint->r2) {
        HitPoint *hp = p->hitpoint;
        double factor = (hp->n * Math::alpha + Math::alpha) / (hp->n * Math::alpha + 1.);
        Vector3f dr = d - hp->norm * (2 * Vector3f::dot(d, hp->norm));    
        double rho = hp->brdf.rho_d + hp->brdf.rho_s * pow(Vector3f::dot(dr, hp->dir), hp->brdf.phong_s);
        if (rho < 0) rho = 0;
        else if (rho > 1) rho = 1;
        ++hp->n;
        hp->r2 *= factor;
        hp->flux = (hp->flux + hp->weight * weight * rho) * factor;  
    }  
    if (p->ls) update(p->ls, photon, weight, d);
    if (p->rs) update(p->rs, photon, weight, d);
    p->maxr2 = p->hitpoint->r2;
    if (p->ls)
        p->maxr2 = std::max(p->maxr2, p->ls->hitpoint->r2);
    if (p->rs)
        p->maxr2 = std::max(p->maxr2, p->rs->hitpoint->r2);
}

KDTree::~KDTree() {
    if (!root) return;
    del(root);
    delete [] hitpoints;
}

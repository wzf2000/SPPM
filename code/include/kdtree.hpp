#include <vector>

#include "camera.hpp"
#include "group.hpp"
#include "hitpoint.hpp"

struct KDTreeNode {
    HitPoint *hitpoint;
    Vector3f minCoord, maxCoord;
    double maxr2;
    KDTreeNode *ls, *rs;
};

class KDTree {
    int n;
    HitPoint** hitpoints;
    KDTreeNode* build(int l, int r, int d);
    void del(KDTreeNode *p);
public:
    KDTreeNode *root;
    KDTree(std::vector<HitPoint*> hitpoints);
    ~KDTree();
    void update(KDTreeNode *p, Vector3f photon, Vector3f weight, Vector3f d);
};

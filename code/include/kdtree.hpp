#include <vector>

#include "camera.hpp"
#include "group.hpp"

struct KDTreeNode {
    Vector3f minCoord, maxCoord;
    int l, r;
    KDTreeNode *ls, *rs;
    Group *group;
};

class KDTree {

};

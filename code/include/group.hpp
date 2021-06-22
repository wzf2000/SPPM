#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() = delete;

    explicit Group (int num_objects) {
        group.resize(num_objects);
    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        bool ret = false;
        for (auto object : group)
            ret |= object->intersect(r, h, tmin);
        return ret;
    }

    void addObject(int index, Object3D *obj) {
        group[index] = obj;
    }

    int getGroupSize() {
        return (int)group.size();
    }

private:
    std::vector<Object3D*> group;
};

#endif
	

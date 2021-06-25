#include "mesh.hpp"
#include "triangle.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <cstring>
#include <vector>

static std::vector<std::string> split(char *str) {
    std::vector<std::string> ret;
    std::string cur = "";
    for (int i = 0; str[i]; ++i) {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r') {
            if (cur != "") ret.emplace_back(cur);
            cur = "";
        }
        else cur += str[i];
    }
    if (cur != "") ret.emplace_back(cur);
    return ret;
}

bool Mesh::intersect(const Ray &r, Hit &h, double tmin) {
    return kdtree->intersect(kdtree->root, r, h, tmin);
}

static std::vector<std::string> mySplit(std::string str, std::string pattern) {
    std::string::size_type pos;
    std::vector<std::string> ret;
    str += pattern;
    int size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            ret.emplace_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return ret;
}

Mesh::Mesh(const char *filename, Material *material, Vector3f *center) : Object3D(material), center(center) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    int len = strlen(filename);
    if (strcmp(".obj", filename + len - 4) == 0) {
        std::string line;
        std::string vTok("v");
        std::string fTok("f");
        std::string texTok("vt");
        std::string vnTok("vn");
        char bslash = '/', space = ' ';
        std::string tok;
        int texID;
        while (true) {
            std::getline(f, line);
            if (f.eof()) {
                break;
            }
            if (line.size() < 3) {
                continue;
            }
            if (line.at(0) == '#') {
                continue;
            }
            std::stringstream ss(line);
            ss >> tok;
            if (tok == vTok) {
                Vector3f vec;
                ss >> vec[0] >> vec[1] >> vec[2];
                v.push_back(vec);
            } else if (tok == fTok) {
                bool tFlag = 1, nFlag = 1;
                TriangleIndex trig, vtID, vnID;
                for (int i = 0; i < 3; ++i) {
                    std::string str;
                    ss >> str;
                    std::vector<std::string> id = mySplit(str, string(1, bslash));
                    trig[i] = stoi(id[0]) - 1;
                    if (id.size() > 1) vtID[i] = stoi(id[1]) - 1;
                    if (id.size() > 2) vnID[i] = stoi(id[2]) - 1;
                }
                t.emplace_back(trig);
                vtIndex.emplace_back(vtID);
                vnIndex.emplace_back(vnID);
            } else if (tok == texTok) {
                Vector2f texcoord;
                ss >> texcoord[0];
                ss >> texcoord[1];
                vt.emplace_back(texcoord);
            } else if (tok == vnTok) {
                Vector3f vec;
                ss >> vec[0] >> vec[1] >> vec[2];
                vn.emplace_back(vec);
            }
        }
    }
    else if (strcmp(".ply", filename + len - 4) == 0) {
        char buffer[1024];
        int n, faces;
        while (f.getline(buffer, 1024)) {
            if (string(buffer) == "end_header") break;
            std::vector<std::string> tokens = split(buffer);
            if (tokens[0] == "element") {
                if (tokens[1] == "vertex")
                    n = std::stoi(tokens[2]);
                else if (tokens[1] == "face")
                    faces = std::stoi(tokens[2]);
            }
        }
        for (int i = 0; i < n; ++i) {
            Vector3f vec;
            f >> vec[0] >> vec[1] >> vec[2];
            f.getline(buffer, 1024);
            v.emplace_back(vec);
        }
        for (int i = 0; i < faces; ++i) {
            TriangleIndex trig, vtID, vnID;
            int num;
            f >> num;
            if (num != 3)
                throw runtime_error("Only trangle are supported!");
            f >> trig[0] >> trig[1] >> trig[2];
            t.emplace_back(trig);
            vtIndex.emplace_back(vtID);
            vnIndex.emplace_back(vnID);
        }
    }
    std::cerr << filename << std::endl;
    computeNormal();

    f.close();
    faces.clear();
    for (int i = 0; i < t.size(); ++i) {
        TriangleIndex &triangleIndex = t[i];
        Triangle *tr = new Triangle(v[triangleIndex[0]], v[triangleIndex[1]], v[triangleIndex[2]], material, this);
        if (vtIndex[i][0] != -1)
            tr->setVT(vt[vtIndex[i][0]], vt[vtIndex[i][1]], vt[vtIndex[i][2]]);
        if (vnIndex[i][0] != -1)
            tr->setVNormal(vn[vnIndex[i][0]], vn[vnIndex[i][1]], vn[vnIndex[i][2]]);
        faces.emplace_back(tr);
    }
    kdtree = new ObjectKDTree(faces);
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}

Vector3f *Mesh::calcCenter() {
    if (center) return center;
    center = new Vector3f(Vector3f::ZERO);
    for (auto &vertex : v)
        *center = *center + vertex;
    *center = *center / v.size();
    return center;
}

Ray Mesh::generateRandomRay() const {
    int index = Math::random() * faces.size();
    return faces[index]->generateRandomRay();
}

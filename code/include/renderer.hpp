#pragma once
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>

#include "tile.hpp"
#include "scene_parser.hpp"
#include "image.hpp"
#include "hit.hpp"
#include "kdtree.hpp"

constexpr int maxDepth = 10;
constexpr double sigma_s = 0.025;
constexpr double sigma_a = 0.015;
constexpr double sigma_t = sigma_s + sigma_a;

class Renderer {
    SceneParser *scene;
    Camera *camera;
    Image *image;
    std::vector<HitPoint*> hitPoints;
    KDTree *kdtree;
    Sphere *media;
    int numPhotons;
public:
    Renderer(SceneParser *);
    void render(int, int, int, std::string);
    void renderPerTile(Tile);
    void trace(const Ray &ray, const Vector3f &weight, int depth, HitPoint *hp = nullptr);
    void initHitKDTree();
    void evaluateRadiance(int numRounds);
    static int count;
};
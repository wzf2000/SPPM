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

const float gamma = 0.5;

class Renderer {
    SceneParser *scene;
    Camera *camera;
    Image *image;
    std::vector<HitPoint*> hitPoints;
    int numPhotons;
    Vector3f light;
    double aperture;
    double focus;
public:
    Renderer(SceneParser *);
    void render(int);
    void renderPerTile(Tile);
    void trace(const Ray &ray, const Vector3f &weight, int depth, HitPoint *hp = nullptr);
    void initHitKDTree();
    void evaluateRadiance(int numRounds);
};
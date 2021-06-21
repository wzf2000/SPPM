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

class Renderer {
    SceneParser *scene;
    Camera *camera;
    Image *image;
    std::vector<Hit> hitPoints;
public:
    Renderer(SceneParser *);
    void renderPerTile(Tile);
    void trace();
};
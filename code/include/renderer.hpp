#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "tile.hpp"
#include "scene_parser.hpp"
#include "image.hpp"

struct State {
    Tile *tiles;
    int tileCount;
    int finishedTileCount;
};

struct Prefs {
    int numPhotons;
    unsigned tileWidth;
    unsigned tileHeight;

    unsigned imageWidth;
    unsigned imageHeight;
};

class Renderer {
    State state;
    Prefs prefs;
    SceneParser *scene;
    Image *image;
public:
    void renderPerTile(Tile);
};
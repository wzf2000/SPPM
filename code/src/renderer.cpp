#include "renderer.hpp"
#include "tile.hpp"
#include "camera.hpp"
#include "sampler.hpp"
#include "math.hpp"
#include "plane.hpp"

void Renderer::renderPerTile(Tile tile) {
    Sampler *sampler = nullptr;
    Camera *camera = scene->getCamera();
    for (int y = tile.end.y - 1; y >= tile.begin.y; --y) {
        for (int x = tile.begin.x; x < tile.end.x; ++x) {
            Ray camRay = camera->generateRay(Vector2f(x, y));

        }
    }
}
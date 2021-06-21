#include "renderer.hpp"
#include "tile.hpp"
#include "camera.hpp"
#include "sampler.hpp"
#include "math.hpp"
#include "plane.hpp"

Renderer::Renderer(SceneParser *scene) : scene(scene) {
    camera = scene->getCamera();
    image = new Image(camera->getWidth(), camera->getHeight());
    for (int i = 0; i < image->Width(); ++i)
        for (int j = 0; j < image->Height(); ++j)
            hitPoints.emplace_back();
}

void Renderer::renderPerTile(Tile tile) {
    Sampler *sampler = nullptr;
    Camera *camera = scene->getCamera();
    for (int y = tile.end.y - 1; y >= tile.begin.y; --y) {
        for (int x = tile.begin.x; x < tile.end.x; ++x) {
            Ray camRay = camera->generateRay(Vector2f(x, y));
            hitPoints[y * image->Height() + x] = Hit();
            trace();
        }
    }
}

void Renderer::trace() {
    
}

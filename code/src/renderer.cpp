#include "renderer.hpp"
#include "tile.hpp"
#include "camera.hpp"
#include "sampler.hpp"
#include "math.hpp"
#include "plane.hpp"
#include "hitpoint.hpp"
#include "triangle.hpp"

Renderer::Renderer(SceneParser *scene) : scene(scene), light(1), aperture(1e-3), focus(1.09), kdtree(nullptr) {
    camera = scene->getCamera();
    image = new Image(camera->getWidth(), camera->getHeight());
    for (int i = 0; i < image->Width(); ++i)
        for (int j = 0; j < image->Height(); ++j)
            hitPoints.emplace_back(new HitPoint());
}

void Renderer::evaluateRadiance(int numRounds) {
    for (int y = 0; y < image->Width(); ++y)
        for (int x = 0; x < image->Height(); ++x) {
            HitPoint *hp = hitPoints[y * image->Height() + x];
            Vector3f color =  hp->flux / (M_PI * hp->r2 * numPhotons * numRounds) + light * hp->fluxLight / numRounds;
            color.x() = pow(color.x(), gamma);
            color.y() = pow(color.y(), gamma);
            color.z() = pow(color.z(), gamma);
            image->SetPixel(x, y, min(color, Vector3f(1)));
    }
}

void Renderer::render(int numRounds, std::string output) {
    for (int i = 0; i < numRounds; ++i) {
        renderPerTile((Tile){(intCoord){0, 0}, (intCoord){image->Height(), image->Width()}});
        if ((i + 1) % 100 == 0) {
            evaluateRadiance(i + 1);
            char filename[100];
            sprintf(filename, "output/checkpoint-%d.ppm", i + 1);
            image->SaveImage(filename);
        }
    }
    image->SaveImage(output.c_str());
}

void Renderer::renderPerTile(Tile tile) {
    Triangle triangle(Vector3f(0, 0, camera->center.z() + focus), Vector3f(0, 1, camera->center.z() + focus), Vector3f(1, 0, camera->center.z() + focus), nullptr);
    for (int y = tile.end.y - 1; y >= tile.begin.y; --y) {
        for (int x = tile.begin.x; x < tile.end.x; ++x) {
            Ray camRay = camera->generateRay(Vector2f(x, y));
            double t = triangle.intersectPlane(camRay);
            Vector3f focusP = camRay.getOrigin() + camRay.getDirection() * t;
            double theta = Math::random(0, 2 * M_PI);
            Ray ray(camRay.getOrigin() + Vector3f(cos(theta), sin(theta), 0) * aperture, (focusP - ray.getOrigin()).normalized());
            hitPoints[y * image->Height() + x]->valid = false;
            hitPoints[y * image->Height() + x]->dir = -1 * camRay.getDirection();
            trace(ray, Vector3f(1), 1, hitPoints[y * image->Height() + x]);
        }
    }
    initHitKDTree();
    Vector3f weight_init = 2.5;
    for (int i = 0; i < numPhotons; ++i) {
        Ray camRay = camera->generateRay();
        trace(camRay, weight_init * light, 1);
    }
}

void Renderer::trace(const Ray &ray, const Vector3f &weight, int depth, HitPoint *hp) {
    if (depth > maxDepth) return;
    double tmin = 1e100;
    Group *group = 
}

void Renderer::initHitKDTree() {
    if (kdtree) delete kdtree;
    kdtree = new KDTree(hitPoints);
}

#include "renderer.hpp"
#include "tile.hpp"
#include "camera.hpp"
#include "sampler.hpp"
#include "math.hpp"
#include "plane.hpp"
#include "hitpoint.hpp"
#include "triangle.hpp"
#include "brdf.hpp"
#include "mesh.hpp"

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
            sprintf(filename, "checkpoints/checkpoint-%d.ppm", i + 1);
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
    Group *group = scene->getGroup();
    Hit hit;
    bool flag = group->intersect(ray, hit, 0);
    Vector3f p = ray.getOrigin() + ray.getDirection() * hit.getT();
    double s = BRDFs[hit.getMaterial()->brdf].specular + BRDFs[hit.getMaterial()->brdf].diffuse + BRDFs[hit.getMaterial()->brdf].refraction;
    double action = s * Math::random(0, 1);
    Vector3f dr = ray.getDirection() - hit.getNormal() * (2 * Vector3f::dot(ray.getDirection(), hit.getNormal()));
    if (BRDFs[hit.getMaterial()->brdf].specular > 0 && action <= BRDFs[hit.getMaterial()->brdf].specular) {
        trace(Ray(p + dr * Math::eps, dr), weight * hit.getMaterial()->texture->query(p) * s, depth + 1, hp);        
        return;
    }
    action -= BRDFs[hit.getMaterial()->brdf].specular;

    if (BRDFs[hit.getMaterial()->brdf].diffuse > 0 && action <= BRDFs[hit.getMaterial()->brdf].diffuse) {
        if (hp) {
            hp->p = p;
            hp->weight = weight * hit.getMaterial()->texture->query(p) * s;
            hp->fluxLight = hp->fluxLight + hp->weight * (hit.getMaterial()->brdf == LIGHT);
            hp->brdf = BRDFs[hit.getMaterial()->brdf];
            hp->norm = hit.getNormal();
            if (hit.getMaterial()->brdf == LIGHT) {
                hp->fluxLight = hp->fluxLight + hp->weight;
                hp->valid = false;
            }
            else
                hp->valid = true;
        }   
        else {
            double a = Math::random();
            // phong specular
            if (a <= BRDFs[hit.getMaterial()->brdf].rho_s) { 
                Vector3f d = Math::sampleReflectedRay(dr, BRDFs[hit.getMaterial()->brdf].phong_s);
                trace(Ray(p + d * Math::eps, d), weight * hit.getMaterial()->texture->query(p) * s, depth + 1, hp);
            }
            else {
                a -= BRDFs[hit.getMaterial()->brdf].rho_s;
                kdtree->update(kdtree->root, p, weight, ray.getDirection());
                Vector3f d = Math::sampleReflectedRay(hit.getNormal());
                if (Vector3f::dot(d, hit.getNormal()) < 0) d = d * -1;
                if (a <= BRDFs[hit.getMaterial()->brdf].rho_d) {
                    trace(Ray(p + d * Math::eps, d), weight * hit.getMaterial()->texture->query(p) * s, depth + 1, hp);
                }   
            }
        }    
        return;
    }
    action -= BRDFs[hit.getMaterial()->brdf].diffuse;

    if (BRDFs[hit.getMaterial()->brdf].refraction > 0 && action <= BRDFs[hit.getMaterial()->brdf].refraction) {
        double refractiveIndex = BRDFs[hit.getMaterial()->brdf].refractiveIndex;
        bool incoming = false;
        if (hit.object) {
            if (!hit.object->center) 
                hit.object->calcCenter();
            incoming = Vector3f::dot(*(hit.object->center) - p, hit.getNormal()) < 0;
            if (!incoming) refractiveIndex = 1. / refractiveIndex;
        }
        
        double cosThetaIn = -Vector3f::dot(ray.getDirection(), hit.getNormal());
        double cosThetaOut2 = 1 - (1 - Math::sqr(cosThetaIn)) / Math::sqr(refractiveIndex);
        
        if (cosThetaOut2 >= -Math::eps) {
            double cosThetaOut = sqrt(cosThetaOut2);

            double R0 = Math::sqr((1 - refractiveIndex) / (1 + refractiveIndex));
            double cosTheta = incoming ? cosThetaIn : cosThetaOut;
            double R = R0 + (1 - R0) * pow(1 - cosTheta, 5);
            
            if (Math::random() <= R)
                trace(Ray(p + dr * Math::eps, dr), weight * hit.getMaterial()->texture->query(p) * s, depth + 1, hp);
            else {
                Vector3f d = ray.getDirection() / refractiveIndex + hit.getNormal() * (cosThetaIn / refractiveIndex - cosThetaOut);
                trace(Ray(p + d * Math::eps, d), weight * hit.getMaterial()->texture->query(p) * s, depth + 1, hp);                                    
            }
        }
        else {
            trace(Ray(p + dr * Math::eps, dr), weight * hit.getMaterial()->texture->query(p) * s, depth + 1, hp);        
        }
    }
}

void Renderer::initHitKDTree() {
    if (kdtree) delete kdtree;
    kdtree = new KDTree(hitPoints);
}

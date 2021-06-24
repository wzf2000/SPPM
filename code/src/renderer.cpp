#include "renderer.hpp"
#include "tile.hpp"
#include "camera.hpp"
#include "math.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "hitpoint.hpp"
#include "triangle.hpp"
#include "brdf.hpp"
#include "mesh.hpp"
#include <utility>

#ifdef VOLUMETRIC_OPEN
#define VOLUMETRIC 1
#else
#define VOLUMETRIC 0
#endif

int Renderer::count = 0;

Renderer::Renderer(SceneParser *scene) : scene(scene), aperture(1e-3), focus(1.09), kdtree(nullptr), media(new Sphere(Vector3f(2, 2, 3), 10, new Material)) {
    numPhotons = 1000000;
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
            Vector3f color =  hp->flux / (M_PI * hp->r2 * numPhotons * numRounds) + hp->fluxLight / numRounds;
            color.x() = std::pow(color.x(), Math::gamma);
            color.y() = std::pow(color.y(), Math::gamma);
            color.z() = std::pow(color.z(), Math::gamma);
            // if (color != Vector3f::ZERO)
                // ++count;
            image->SetPixel(y, x, min(color, Vector3f(1)));
    }
}

void Renderer::render(int numRounds, std::string output) {
    for (int i = 0; i < numRounds; ++i) {
        count = 0;
        fprintf(stderr, "Round %d/%d:\n", i + 1, numRounds);
        renderPerTile((Tile){(intCoord){0, 0}, (intCoord){image->Height(), image->Width()}});
        if ((i + 1) % 1 == 0) {
            evaluateRadiance(i + 1);
            char filename[100];
            sprintf(filename, "checkpoints/checkpoint-%d.bmp", i + 1);
            image->SaveImage(filename);
        }
        std::cerr << count << std::endl;
    }
    image->SaveImage(output.c_str());
}

void Renderer::renderPerTile(Tile tile) {
    for (int y = tile.begin.y; y < tile.end.y; ++y) {
        fprintf(stderr, "\rRay tracing pass %.3lf%%", y * 100. / image->Width());
        #pragma omp parallel for schedule(dynamic, 1), num_threads(8)
        for (int x = tile.begin.x; x < tile.end.x; ++x) {
            Ray ray = camera->generateRandomRay(Vector2f(y, x));
            hitPoints[y * image->Height() + x]->dir = -ray.getDirection();
            trace(ray, Vector3f(1), 1, hitPoints[y * image->Height() + x]);
        }
    }
    fprintf(stderr, "\rRay tracing pass 100.000%%\n");
    initHitKDTree();
    #pragma omp parallel for schedule(dynamic, 1), num_threads(8)
    for (int i = 0; i < numPhotons; ++i) {
        std::pair<Ray, Vector3f> camRay = scene->generateRay();
        trace(camRay.first, camRay.second, 1);
    }
    fprintf(stderr, "\rPhoton tracing pass done\n");
}

static inline double sample_segment(double eps, double s_max) {
    return -log(1. - eps * (1. - (exp(-sigma_s * s_max))));
}

static inline Vector3f sampleHG(double g, double e1, double e2) {
    double s = 1. - 2. * e1;
    double cost = (s + 2. * Math::sqr(g) * g * (e1 - 1.) * e1 + Math::sqr(g) * s + 2. * g * (1 - e1 + Math::sqr(e1))) / Math::sqr(1. + g * s);
    double sint = sqrt(1. - Math::sqr(cost));
    return Vector3f(cos(2.0 * M_PI * e2) * sint, sin(2.0 * M_PI * e2) * sint, cost);
}

static inline void generate_cartesian(Vector3f &u, Vector3f &v, const Vector3f &w) {
    Vector3f coVec;
    if (fabs(w.x()) <= fabs(w.y())) {
        if (fabs(w.x()) <= fabs(w.z())) coVec = Vector3f(0, -w.z(), w.y());
        else coVec = Vector3f(-w.y(), w.x(), 0);
    }
    else {
        if (fabs(w.y()) < fabs(w.z())) coVec = Vector3f(-w.z(), 0, w.x());
        else coVec = Vector3f(-w.y(), w.x(), 0);
    }
    coVec.normalize();
    u = Vector3f::cross(w, coVec);
    v = Vector3f::cross(w, u);
}

static inline double scatter(const Ray &r, Ray *&scatter_ray, double t_near, double t_far, double &s) {
    s = sample_segment(Math::random(), t_far - t_near);
    Vector3f x = r.pointAtParameter(t_near) + r.getDirection() * s;
    Vector3f dir = sampleHG(-0.5, Math::random(), Math::random());
    Vector3f u, v;
    generate_cartesian(u, v, r.getDirection());
    dir = u * dir.x() + v * dir.y() + r.getDirection() * dir.z();
    scatter_ray = new Ray(x, dir);
    return (1. - exp(-sigma_s * (t_far - t_near)));
}

void Renderer::trace(const Ray &ray, const Vector3f &weight, int depth, HitPoint *hp) {
    if (depth > maxDepth) return;
    double t_near, t_far;
    double scale = 1.0;
    double absorption = 1.0;
    Group *group = scene->getGroup();
    if (VOLUMETRIC) {
        bool flag_media = media->intersect_media(ray, t_near, t_far);
        if (flag_media) {
            Ray *scatter_ray;
            Hit hh;
            double s;
            double prob_s = scatter(ray, scatter_ray, t_near, t_far, s);
            scale = 1 / (1. - prob_s);
            double chance = Math::random();
            if (prob_s >= chance) {
                hh.set(t_near + s, nullptr, Vector3f(0));
                if (!group->intersect(ray, hh, 0)) {
                    trace(*scatter_ray, weight, depth + 1, hp);
                    delete scatter_ray;
                    return;
                }
            }
            else {
                if (!group->intersect(ray, hh, 0)) {
                    if (hp) hp->fluxLight += hp->weight * scene->getBackgroundColor();
                    delete scatter_ray;
                    return;
                }
            }
            if (hh.getT() >= t_near) {
                double dis = std::min(hh.getT(), t_far) - t_near;
                absorption = exp(-sigma_t * dis);
            }
            delete scatter_ray;
        }
    }
    Hit hit;
    bool flag = group->intersect(ray, hit, 0);
    if (!hit.getMaterial() || !hp && hit.getT() < 1e-3) {
        if (hp) hp->fluxLight += hp->weight * scene->getBackgroundColor();
        return;
    }
    Vector3f p = ray.pointAtParameter(hit.getT());
    bool incoming = true;
    if (hit.center) {
        incoming = Vector3f::dot(*(hit.center) - p, hit.getNormal()) < 0;
    }

    double ss = scale;
    double action = Math::random();
    Vector3f dr = ray.getDirection() - hit.getNormal() * (2 * Vector3f::dot(ray.getDirection(), hit.getNormal()));
    // Specular
    if (BRDFs[hit.getMaterial()->brdf].specular > 0 && action <= BRDFs[hit.getMaterial()->brdf].specular) {
        if (incoming) ss = ss * absorption;
        trace(Ray(p, dr), weight * hit.getMaterial()->texture->query(p) * ss, depth + 1, hp);
        return;
    }
    action -= BRDFs[hit.getMaterial()->brdf].specular;

    // Diffuse
    if (BRDFs[hit.getMaterial()->brdf].diffuse > 0 && action <= BRDFs[hit.getMaterial()->brdf].diffuse) {
        if (!incoming) ss = ss * absorption;
        if (hp) {
            // ++count;
            hp->p = p;
            hp->weight = weight * hit.getMaterial()->texture->query(p) * ss;
            hp->fluxLight = hp->fluxLight + hp->weight * hit.getMaterial()->emission;
            hp->brdf = BRDFs[hit.getMaterial()->brdf];
            hp->norm = hit.getNormal();
        }
        else {
            double a = Math::random();
            // phong specular
            if (a <= BRDFs[hit.getMaterial()->brdf].rho_s) {
                Vector3f d = Math::sampleReflectedRay(dr, BRDFs[hit.getMaterial()->brdf].phong_s);
                trace(Ray(p, d), weight * hit.getMaterial()->texture->query(p) * ss, depth + 1, hp);
            }
            else {
                // ++count;
                a -= BRDFs[hit.getMaterial()->brdf].rho_s;
                kdtree->update(kdtree->root, p, weight, ray.getDirection());
                Vector3f d = Math::sampleReflectedRay(hit.getNormal());
                if (Vector3f::dot(d, hit.getNormal()) < 0) d = d * -1;
                if (a <= BRDFs[hit.getMaterial()->brdf].rho_d) {
                    trace(Ray(p, d), weight * hit.getMaterial()->texture->query(p) * ss, depth + 1, hp);
                }
            }
        }
        return;
    }
    action -= BRDFs[hit.getMaterial()->brdf].diffuse;

    // Refraction
    if (BRDFs[hit.getMaterial()->brdf].refraction > 0 && action <= BRDFs[hit.getMaterial()->brdf].refraction) {
        double refractiveIndex = BRDFs[hit.getMaterial()->brdf].refractiveIndex;
        if (!incoming) refractiveIndex = 1. / refractiveIndex;
        
        double cosThetaIn = -Vector3f::dot(ray.getDirection(), hit.getNormal());
        double cosThetaOut2 = 1 - (1 - Math::sqr(cosThetaIn)) / Math::sqr(refractiveIndex);
        
        if (cosThetaOut2 > 0) {
            double cosThetaOut = sqrt(cosThetaOut2);

            double R0 = Math::sqr((1 - refractiveIndex) / (1 + refractiveIndex));
            double cosTheta = incoming ? cosThetaIn : cosThetaOut;
            double R = R0 + (1 - R0) * pow(1 - cosTheta, 5);
            
            if (Math::random() <= R)
                trace(Ray(p, dr), weight * hit.getMaterial()->texture->query(p) * ss, depth + 1, hp);
            else {
                Vector3f d = ray.getDirection() / refractiveIndex + hit.getNormal() * (cosThetaIn / refractiveIndex - cosThetaOut);
                trace(Ray(p, d), weight * hit.getMaterial()->texture->query(p) * ss, depth + 1, hp);
            }
        }
        else {
            trace(Ray(p, dr), weight * hit.getMaterial()->texture->query(p) * ss, depth + 1, hp);
        }
    }
}

void Renderer::initHitKDTree() {
    if (kdtree) delete kdtree;
    kdtree = new KDTree(hitPoints);
}

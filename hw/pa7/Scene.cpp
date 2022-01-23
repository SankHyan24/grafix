//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

void Scene::buildBVH()
{
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum)
            {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
    const Ray &ray,
    const std::vector<Object *> &objects,
    float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear)
        {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TODO Implement Path Tracing Algorithm here
    // intersect(const Ray ray) in Scene.cpp
    // : 求一条光线与场景的交点
    // sampleLight(Intersection pos, float pdf) in Scene.cpp
    // : 在场景的所有光源上按面积 uniform 地 sample 一个点，并计算该 sample 的概率密度
    // sample(const Vector3f wi, const Vector3f N) in Material.cpp
    // : 按照该材质的性质，给定入射方向与法向量，用某种分布采样一个出射方向
    // pdf(const Vector3f wi, const Vector3f wo, const Vector3f N) in Material.cpp
    // : 给定一对入射、出射方向与法向量，计算 sample 方法得到该出射方向的概率密度
    // eval(const Vector3f wi, const Vector3f wo, const Vector3f N) in Material.cpp
    // : 给定一对入射、出射方向与法向量，计算这种情况下的 f_r
    // RussianRoulette in Scene.cpp : P_RR,  Russian Roulette 的概率

    Vector3f L_dir(0, 0, 0);   // Result
    Vector3f L_indir(0, 0, 0); // Result
    // If the ray get the intersection with the sence
    Intersection inter = intersect(ray);
    if (!inter.happened)
        return Vector3f(0, 0, 0);
    if (inter.m->hasEmission())
    {
        if (depth == 0)
            return inter.m->getEmission();
        else
            return Vector3f(0, 0, 0);
    }
    // If light intersect with the point directly
    Intersection lightinter;
    float pdf_light;
    sampleLight(lightinter, pdf_light);
    auto &pointN = inter.normal;
    auto &pointC = inter.coords;
    auto &VlightN = lightinter.normal;
    auto &VlightC = lightinter.coords;
    auto VlightDir = (VlightC - pointC).normalized();
    Ray Vlight(pointC, VlightDir); // Define the virtual light
    Intersection Vlightinter = intersect(Vlight);
    // Light hit the point directly
    if (Vlightinter.happened && (Vlightinter.coords - VlightC).norm() < 1e-2)
    {
        Vector3f distvector = (pointC - VlightC);
        float dist_2p = distvector.x * distvector.x + distvector.y * distvector.y + distvector.z * distvector.z;
        L_dir = lightinter.emit * inter.m->eval(ray.direction, VlightDir, pointN) * dotProduct(VlightDir, pointN) * dotProduct(-VlightDir, VlightN) / dist_2p / pdf_light;
    }
    if (get_random_float() < RussianRoulette)
    {
        Vector3f wi = inter.m->sample(ray.direction, pointN).normalized();
        Ray raytrace(pointC, wi);
        Intersection traceinter = intersect(raytrace);
        if (traceinter.happened && !traceinter.m->hasEmission())
        {
            Vector3f ind_castray = castRay(raytrace, depth + 1);
            Vector3f ind_f_r = inter.m->eval(ray.direction, wi, pointN);
            float ind_pdf = inter.m->pdf(ray.direction, wi, pointN);
            L_indir = ind_castray * ind_f_r * dotProduct(wi, pointN) / ind_pdf / RussianRoulette;
        }
    }
    return L_indir + L_dir;
}
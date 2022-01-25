//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection()
    {
        happened = false;
        coords = Vector3f();
        normal = Vector3f();
        distance = std::numeric_limits<double>::max();
        obj = nullptr;
        m = nullptr;
    }
    bool happened;    // 是否发生相交事件
    Vector3f coords;  // 相交点坐标
    Vector3f tcoords; // ？？？
    Vector3f normal;  // 相交平面法向量
    Vector3f emit;    // 散射？
    double distance;  // 距离，类似于Z-buffer
    Object *obj;      // 弹射相交的物体
    Material *m;      // 弹射相交的材料
};
#endif //RAYTRACING_INTERSECTION_H

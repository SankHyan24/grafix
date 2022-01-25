//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H
#include "Vector.hpp"
struct Ray
{
    //Destination = origin + t*direction
    Vector3f origin;                   // 光线出发点
    Vector3f direction, direction_inv; //光线出射方向和方向的倒数
    double t;                          //transportation time,
    double t_min, t_max;               //时间的合理范围区间

    Ray(const Vector3f &ori, const Vector3f &dir, const double _t = 0.0) : origin(ori), direction(dir), t(_t)
    {
        /*这个构造函数的冒号，意思是构造ray类的成员变量类*/
        direction_inv = Vector3f(1. / direction.x, 1. / direction.y, 1. / direction.z);
        t_min = 0.0;
        t_max = std::numeric_limits<double>::max();
    }

    Vector3f operator()(double t) const { return origin + direction * t; }

    friend std::ostream &operator<<(std::ostream &os, const Ray &r)
    {
        os << "[origin:=" << r.origin << ", direction=" << r.direction << ", time=" << r.t << "]\n";
        return os;
    }
};
#endif //RAYTRACING_RAY_H

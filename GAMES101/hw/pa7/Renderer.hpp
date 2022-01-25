//
// Created by goksu on 2/25/20.
//
#include "Scene.hpp"

#pragma once
struct hit_payload
{
    float tNear;
    uint32_t index;
    Vector2f uv;
    Object *hit_obj;
};

class Renderer // 渲染器类
{
public:
    void Render(const Scene &scene); // 对场景进行渲染

private:
};

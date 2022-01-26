# Lecture 1&2 Overview & Recap of CG Basis
## Contents
- Shadows
- Global Illum
- Physically-based Shading
- Real-time Ray Tracing

## Graphics Pipeline [渲染管线 复习](https://scitbb.top/2022/01/14/2201141/)

## OpenGL:
- 一系列通过CPU调用GPU的API
    - 语言不重要
    - 跨平台
    - 替代品：DirectX，Vulkan
- 缺点：
    - 版本比较碎片化
    - C style, 使用不是很简单
    - （之前）不好Debug
- 如何理解：
    - 和101里面的光栅化一一对应
- 放置/摆放物体，视图变换啥的，GL有矩阵相乘的API 
- VBO 存储模型的向量、坐标、纹理坐标
    - 之前的obj文件
- 使用一个画架easel（GL里面的framebuffer）可以渲染多张图
- 还有一种是直接渲染到屏幕（垂直同步），但是容易造成画面撕裂。使用双/三重缓冲避免
- 目前只需要用到vertex shader和fragment shader

### 大致渲染过程，每一次pass
- 指定物体、相机、MVP等等
- 指定framebuffer、输入输出texture
- 指定vertex/fragment shader
- 开始Render

> OpenGL实质上使用状态机模型实现

### 着色语言的历史：HLSL






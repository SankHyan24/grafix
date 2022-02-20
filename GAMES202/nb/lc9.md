# Lecture 9 Real-time Global Illumination 3 (screen space cont.)
## Screen Space Directional Occlusion (SSDO)
What is SSDO?
- SSAO加强版，考虑了间接光照

Key Idea
- 和Path Tracing很像
- 在shading point，随即发射射线，如果没有照射到障碍物，就是直接光照（AO认为是间接光照）
- 如果照射到障碍物，就是间接光照（AO认为是没有光照） 
 
 实际使用：
 - 从shading point出射一个半球，选一系列采样点。
 - 用采样点从camera的可见性来近似为从p的可见性
 - 如果采样点被挡住，就是算间接光照；否则取直接光照

 问题：
 - 只能应对小范围的GI，不能得到远距离的间接光照
 - 从screen space研究，肯定会丢失相关信息

 ## Screen Space Reflection (SSR)
 What is SSR?
 - 实时渲染中的一种引入全局光照的方法
 - 使用了光线追踪
 - 仍在Screen Space中研究问题

 怎么做？
 - Shade Sence + Nromal Vector + Depth -> SSR

先求交点：intersection
- 使用Linear Raymarch：
  - 每一步检查深度值
  - 结果的质量取决于步长大小 
  - 可以优化
- 使用Depth MipMap
  - depth存四个值的最小值：接触不到最小的，肯定也不可能接触到所有的点
  - 很像在平面空间里的KD-tree，或者说是一种预计算
- Hierarchy tracing，顺次查询mipmap，快速求交点

最大的问题还是screen space的问题
- 几何图形遮挡：camera看不到的地方没有反射
- 边缘割裂：图像边缘外的地方不能产生反射（用模糊edge fading解决）

Pros
- glossy和specular比较容易做
- 质量高，速度快
- 没有spikes（边界5 亮点）和遮蔽问题

Cons
- diffuse不太容易做
- screen space丢失信息
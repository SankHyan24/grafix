# Lecture 8 Real-time Global Illumination 2 (screen space)
## Light Propagation Volumes (LPV)
Key Problem 主要问题
- 要实现任何一个Shading Point的间接光照，要先知道这一个点的Radiance

Key Idea 主要思路
- 在一个直线上传播的Radiance（单位立体角单位面积接收到的能量）是一个不变的量。

Key Solution 主要方法
- 用一个3D网格，作为传播介质，来传播从物体表面到任何其他地方的直接照射的radiance。

Step 步骤
- 首先生成一系列点，就是被直接光照照射到的点
  - 用RSM就足够了，但是还是存在计算量的问题
- 将这些虚拟光源的点集合“注入”到格子（Radiance Volumn）里面
  - 先将场景预处理分成一个个的格子
  - 对每一个格子，计算相邻的source light
  - 将直接光照的分布加起来
  - 把得到的次级光照分布使用前两阶（4terms）SH进行压缩
- 用Volumn Radiance 进行光线传播计算
  - 对于每个格子，收集6个方向的radiance
  - 加起来，用SH表达
  - 最后迭代几次直到数值稳定
- 渲染场景，得到结果

问题：
- 漏光Light Leaking 原因是不考虑一个格子的光线遮挡问题

## Voxel Global Illumination (VXGI)
也是一个2pass algorithm。但是和RSM有区别
- 不是RSM一样记录像素，而是记录有Hierarchical的一堆格子
- 区别于RSM的Sample，这里使用类似于RT的Cones Tracing，追踪Glossy物质反射出的圆锥

步骤，第一步pass 1
- 


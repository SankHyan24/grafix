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
- pass1：不是RSM一样记录像素，而是记录有Hierarchical的一堆格子
- pass2：区别于RSM的Sample，camera ray这里使用类似于RT的Cones Tracing（光锥追踪），追踪Glossy物质反射出的圆锥

步骤，第一步pass 1
- 类似于预计算，在每个体素（Voxel）上记录下入射光照及其表面反射分布
- 按照层级从低到高逐次更新

第二部pass 2
- 对于Glossy的表面，跟踪从表面反射出来的cone
- 根据hierarchy逐层查询voxel和cone的关系，计算其contribution，得到shading point的信息
- 对于diffuse的物体，可以看成表面反射出多个cones，覆盖到整个反射面。中间反射漏的位置可以不考虑。

## Real-time Global Illumination - Screen Space
What is Screen Space?
- 只在屏幕中显示出来的信息
- 换句话说，是对现有渲染结果的后续处理

### Screen Space Ambient Occlusion 环境光遮蔽
为什么用AO？
- 开销小，容易实现一些
- 而且能突出物体之间的相对位置

什么是SSAO？
- 是一种对全局光照GI的一种近似
- 只针对于屏幕空间内

Key Idea 1
- 我们不知道入射的间接光
- 所以我们猜想它是一个常数
- （布林冯模型）

Key Idea 2 & 3
- 考虑到**不同着色点各个方向的Visibility**，有的地方接收不到来自一些方向的间接光照
- 另外，只将其考虑为Diffuse（粗糙）的物体

IN THEORY，从Rendering Equation来看
- 渲染方程 $L_o(p,\omega_o)=\int_{\Omega^+}L_i(p,\omega_i)f_r(p,\omega_i,\omega_o)V(p,\omega_i)cos\theta_i\mathrm{d}\omega_i $
- 使用Approx抽出Visibility项
- $L_o(p,\omega_o)\approx\frac{\int_{\Omega^+}V(p,\omega_i)cos\theta_i\mathrm{d}\omega_i}{\int_{\Omega^+}cos\theta_i\mathrm{d}\omega_i}\int_{\Omega^+}L_i^{indir}(p,\omega_i)f_r(p,\omega_i,\omega_o)cos\theta_i \mathrm{d}\omega_i $
- 其中： $\frac{\int_{\Omega^+}V(p,\omega_i)cos\theta_i\mathrm{d}\omega_i}{\int_{\Omega^+}cos\theta_i\mathrm{d}\omega_i}=k_A=\frac{\int_{\Omega^+}V(p,\omega_i)cos\theta_i \mathrm{d}\omega_i}{\pi}$
- 其中： $\int_{\Omega^+}L_i^{indir}(p,\omega_i)f_r(p,\omega_i,\omega_o) \mathrm{d}\omega_i =L_i^{indir}(p)\cdot\rho$ （对AO来讲是一个常数）
- 就是平均的一个visibility乘上一个数（为啥这个不等式里面有两个cos，是吧cos和dx项当作成了一个整体了）

理论上讲应用，就是shading point 对周围光追一下，看看一定距离内有没有遮挡，有遮挡的就暗。

#### Use Z-buffer
但是实际上的AO，sample shading point周围一个球体内的许多点，看哪些点能被shading point“看到”。

有很多细节，就是Screen Space里除了存z-buffer，还存不存反射面法线。不存法线出现的问题：不该变暗的边缘变暗，“错误遮挡”。使用法线的SSAO叫做HBAO，效果更好。

sample的球的大小？思想可以也和PCSS一样，adaptive化。也可以用小的sample sphere，再用降噪算法处理。

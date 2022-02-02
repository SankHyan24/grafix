# Lecture 5 Real-time Enviroment Mapping
## Distance Field Soft Shadow (DFSS)
### 先复习了一下GAMES101的Distance Function
在几何1里面讲的
- sdf有向距离场

### Distance Field的运用
1. Ray marching (sphere tracing) to perform ray-SDF intersection
  - 通过sdf给出的“安全距离”，
2. Use SDF to determine the (approx.) percentage of occlusion
  - a “safe” angle seen from the eye 从安全距离得到安全角度（最小的那个）
  - 安全角度越小，阴影就越黑
3. 怎么算安全角：
  - 不用 $arcsin(\frac{SDF(p)}{|p-o|})$ 而是用 $min\{\frac{k\cdot SDF(p)}{|p-o|},1.0\}$

优点：
- 快（但是生成也需要时间）
- 高质量

缺点：
- 需要预计算
- 需要存储
- 会有artifact

## Shading from Environment Lighting 环境光照
技术：Image-Based Lighting (IBL) 从所有方向都有光照，取决于IBL。同样的解一个rendering equation，不过没有Visibility项。

怎么做？Monte Carlo 积分
- 一种数值上的无偏估计
- 需要大量的取样Sample
- 一般来讲不适合用于RTR

所以使用那个约等式：
- $\int_{\Omega}f(x)g(x)dx\approx\frac{\int_{\Omega}f(x)dx}{\int_{\Omega}dx}\cdot \int_{\Omega}g(x)dx$ 
- 因为：如果是glossy的BRDF，就是small support。如果是diffuse的BRDF，就是smooth。都适合使用上面的约等
- 所以能把Rendering Equation里面的Lighting项拆出来.(之前的rendering Equation拆出来的是Visibility项)
- 实际上，前面拆出来的积分是对环境光的滤波（求平均/模糊化）。
  - 所以说，可以先将环境光Prefilter。filter size取决于BRDF覆盖的区域。所以这一步的思想很类似于Mipmap。
  - 查看环境光，就相当于可以直接从看过去的向量的镜面反射方向查询光的信息，就防止了采样
- 现在解决后半部分的积分，对BRDF进行积分。
- 思想：也是一种预计算。但是需要更高维度的table来存。
- [视频在这](https://www.bilibili.com/video/BV1YK4y1T7yY?p=5),时间是1h17m。预计算，把5维变成了个2维的表格，无需Sample，直接预计算，到时候查表。
- 叫做——Split Sum方法。就是分开之后积分。虚幻引擎的技术，因为不用采样，所以没有噪音

## 处理环境光照的遮挡和shadow的介绍
- 可以认为是一个“多光源”问题，如果用SM处理，代价非常高
- 也可以认为是采样的方法，采样光照。但是难点也在于遮挡情况（Visibility项不容易提出来）和采样带来的巨大数量。
- 工业界一般都用General One 方法，也就是取最重要的那个光源来研究处理
- 相关研究：
  - Imperfect Shadow Map
  - Light Cuts
  - RTRT （据说是终极方法，Real Time Ray Tracing=Path Tracing+De-noising）
  - Precomputed Radiance Transfer (PRT)







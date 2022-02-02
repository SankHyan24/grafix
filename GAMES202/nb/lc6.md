# Lecture 6 Real-time Enviroment Mapping 2
## Background Knowledges 一些理解：
**Product-integral**=滤波=卷积=加权平均：
- 卷积 $\int_{\Omega}f(x)g(x)dx$
- 滤波 考虑傅里叶变换，先乘积再积分和频域上的滤波结果相同
- 加权平均，把 $f(x)$ 看成权重函数， $g(x)$ 看成被积函数

**Fourier Transfer** 傅里叶变换：
- 低频信号：分布在频域图中间，表示的是模糊
- 高频信号：分布在周围，表示的更多是图像的边界

**Basis Function** 基函数：
- 可以类比于线性空间的标准正交基。傅里叶级数的每个项都是标准正交基
- 可以用一套基函数的线性组合来表达该函数空间中的任何函数，比如说三角函数代表的傅里叶展开，以及幂级数代表的泰勒展开。

## Real Time Environment Lighting (& global illumination)
### Spherical Harmonics 球面谐波函数（SH）
What 是什么：
- 一系列 定义在球面上的 二维 函数 $B_i(\omega)$
- 另外一个理解：一维情况下的傅里叶函数
- 长得和电子云波函数一样卧槽，每层是l，每层有m=2l+1个基函数。（本质上和电子云波函数概率波长得一样，至少数学上是相通的）
- 每一个SH基函数都对应着一个Legendre多项式

How 怎么用：
- 一个好的性质：SH基函数的系数 $c_i=\int_{\Omega}f(\omega)B_i(\omega)d\omega$ （是Product Integral的形式）求 $c_i$ 的过程叫投影（Projection）
- 用这个基函数的线性组合（当然是Truncated过的）来重组（Reconstraction）想要表达的函数 $f(x)$
- Truncating（也就是只取前几项）的过程，同时也是取低频信号，丢掉高频信号的过程。一举两得。
- 并且diffuse物体其实本来就是一个低通滤波，BRDF一般不超过3阶（9项）。进一步可以写成一个二次型

SH's Properties 球面谐波函数的性质
- Orthonormal 正交性
- Ez Projection 简单投影
  - 套用Product Integral形式的公式  $c_i=\int_{\Omega}f(\omega)B_i(\omega)d\omega$
- Ez Rotation 简单旋转
  - 旋转后的基函数，都能被同阶SH基函数来线性表达
- Ez Convolution 简单卷积
- Few Basis funcitons = Low Freqs 低阶就是低频

### Precomputed Radiance Transfer (PRT)
Diffuse Case:
- $L(o)=\rho\int_{\Omega}L(\mathbf{i})V(\mathbf{i})max(0,\mathbf{n\cdot i})d\mathbf{i}$
- 在场景（物体都不能动，因为V不能变）、机位不发生变化的情况下（只考虑光照发生变化），对环境光照进行展开
- $\approx \rho\sum l_i\int_{\Omega}B_i(\mathbf{i})V(\mathbf{i})max(0,\mathbf{n\cdot i})d\mathbf{i}$
- 其中积分项只与光照方向有关，即可以与计算出来，相当于一个向量
- $\approx\rho\sum l_i\mathbf{T_i}$
- 如果光源发生旋转，可以根据SH的性质，直接对系数进行变换操作，代替预计算结果的变化，实现旋转的效果。

Glossy Case:
- $L(o)=\int_{\Omega}L(\mathbf{i})V(\mathbf{i})\rho(\mathbf{i,o})max(0,\mathbf{n\cdot i})d\mathbf{i}$
- 麻烦的是Glossy的T是o的函数，会比Diffuse的情况多一维。现在的T变成了一个矩阵（不是Diffuse的向量了）对每一个o，都预计算一个向量，合起来作为T
- 带价就是计算量大，并且消耗的存储空间也很大
- 用Glossy的SH一般要用到4阶5阶，不然看上去还是很diffuse


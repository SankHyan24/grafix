# Lecture 3 Real-Time Shadows 2
## A deeper look at pcf
filter/convolution:

 $\[\omega*f\](p)=\sum_{q\in \mathcal{N}}^{}\omega(p,q)f(q)$

in PCSS: 实际上也是一个卷积运算来求得Visibility函数

 $V(x)=\sum_{q\in \mathcal{N} (p)}^{}\omega(p,q)\cdot \chi^+[D_{SM}(q)-D_{sence}(x)]$

PCF是在图像（模型几何结构的二维投影）层面处理，而非是先shadow map之后再在shadow map上面做模糊。

## Revisiting PCSS
PCSS的完整算法
- Step 1 Blocker Search 遮挡物搜索，在一个确切距离中得到平均遮挡深度。
- Step 2 Penumbra Estimate 半影估计，用平均遮挡深度来决定filter size
- Step 3 Percentage Closer Filter 用产生的Adaptive filter size进行PCF

哪一步更慢？
- 第一步和第三步，每次都要看遮挡物，PCF也会比较远近
- 所以有人引进了VSSM

## Variance Soft Shadow Mapping (VSSM)
第一步：遮挡物搜索。其实就是“知道多少分，想知道排第几”的问题
- 简单的办法是知道分布，然后大致知道是排第几。
- 一般都是正态分布，所以实际上就是求均值 $Mean$ 和方差 $Variance$ 的问题

求均值：
- 硬件上：用MIPMAP
- 用SAT（Summed Area Tables）

求方差：
- 使用 $Var(X)=E(X^2)-E^2(X)$
- 怎么用？再生成一个Shadow Map，存的是depth的平方，就容易算出方差

再回到刚才的问题：
- 现在已经弄出了一个正态分布，还有已知的shading点的depth的值
- 已经可以算出周围点depth更大的占比是多少了，现在想计算的是阴影的面积
- 现在就是求高斯函数CDF的准确值（只能求出数值解，要这个数值解尽可能地准确）
  - 一般就查表，但是慢啊
  - 高斯函数就是正态分布的函数图像，不定积分是误差函数（erf）
  - PDF是概率密度函数，CDF是概率分布函数，PDF的积分是CDF

再回到刚才的假设，为什么一定是正态分布？其他分布其实就不准了！但是也没必要那么准。这里用到切比雪夫不等式。
- Chebyshev Inequality (one-tailed version, for t > $\mu$)
- $P(x>t)\leq \frac{\sigma^2}{\sigma^2+(t-\mu)^2}$
- 给一个上分位数，能给出概率的上界。t越大，右部分的概率面积越小，给右半部分一个上界。

现在已经可以把第三步的filter解决了。用该概率就能算出visibility。

运算性能：
- 生成shadow map：可以并行跑
- 时间：
  - 算均值，常数复杂度
  - 算方差，常数复杂度
  - 切比雪夫，常熟复杂度
  - 不需要循环

再来讨论第一步，遮挡物搜索：
- 需要算遮挡物 $z_{occ}$ 的平均深度
- 用公式 $\frac{N_1}{N}z_{occ}+\frac{N_2}{N}z_{unocc}=z_{Avg}$ ，能解出遮挡物平均深度
- 能用切比雪夫不等式估算出 $N1/N=P(x>t)$
- 相应 $N2/N=1-P(x>t)$
- 大胆假设 $z_{unocc}=t$ 
- 因此可以得出 $z_{occ}$

## Mipmap and Summed Area Tables
实现SAT的方式——前缀和(for Range Query)：
- 就是前n个数字的和，预处理数组，先花常数时间把前缀和算出来
- 算一定范围的和，就是两个前缀和相减
- 空间换时间，可以在常数时间得到SAT

## Moment Shadow Mapping (MSM) 矩
用VSSM，其中会使用很多近似，把分布当成正态分布。会产生很多问题：比如，漏光、过暗。过暗还是可以接受的，漏光有点不能接受。

Goal:
- Represent a distrubution more accurately 更精确地描述这个分布，而不是只用Mean和Variance。

Idea:
- Use higher order moments to represent a distribution 用高阶矩描述分布

实际上这就是一个矩估计问题。我们想要求得分布的一个参数，但我们不知道这个分布具体是什么，但是我们知道分布的矩。用高阶矩可以更好的拟合这个分布，矩越多越好是肯定的。（四阶矩和PCF已经差的不是很多了）

> 这不就是矩估计嘛~概统还是很重要的……数学还是很有用的
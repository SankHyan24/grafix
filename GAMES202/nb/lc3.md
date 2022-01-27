# Lecture 3 Real-Time Shadows 1
## Shadow Mapping 生成硬阴影
2 pass Algorithm 渲染两边的算法
- 第一遍，从光源出发，生成sm（Shadow Map）
- 第二遍，从相机出发，使用sm检测其是否在阴影里

一个完全在图像中的算法:
- 优点：一旦图像已经生成，不需要知道图像的几何场景
- 缺点：会产生自遮挡现象，以及锯齿
    - 自遮挡（self occlusion）：Shadow Map中的像素会把一个平面采样分成一块一块的阶梯状深度，但是在从相机出发而言，就会认为，地板之间会发生相互遮挡
    - 处理：添加一个可变的Bias，如果不超出这个bias，就忽略遮挡的影响。（但也相应引入detached shadow不接触的阴影）
    - 另一种方法：second-depth shadow mapping，记录次小深度，用中点深度进行遮挡比较。但是没人用，因为模型不能是全平面，
    - 锯齿（Alias）

> 实时渲染不相信复杂度（哈哈哈哈）（只相信时间，RTR中常数也十分重要）

## Approximation in RTR
实时渲染中的一个重要的约等于，主要是把积分中的两个函数拆开来：
- $\int_{\Omega}f(x)g(x)dx\approx\frac{\int_{\Omega}f(x)dx}{\int_{\Omega}dx}\cdot \int_{\Omega}g(x)dx$ 
- 什么时候越准：实际积分域小 (Small Support)，g(x)足够光滑 (Smooth integrand)

实际使用，对于点光源和方向光源，可以把Visibility项和Shading项分开来：

 $ L_o(p,\omega_o)\approx\frac{\int_{\Omega^+}V(p,\omega_i)d\omega_i}{\int_{\Omega^+}d\omega_i}\int_{\Omega^+}L_i(p,\omega_i)f_r(p,\omega_i\rightarrow\omega_o)cos\theta_i d\omega_i $

什么时候准确？
- Small Support (point / directional lighting)
- Smooth integrand (diffuse brdf / constant radiance area lighting)

## PCF Percentage Closer Filter
做抗锯齿的一个东西，后来演化为专门做软阴影的PCSS

对shading point是否遮挡的结果做一个filter（实际上是求的他们的average）。最终的结果可以将阴影抗锯齿（类似于MSAA，还是会变慢）PCF的filter size越大越模糊，越小阴影越硬

## Percentage Closer Soft Shadows
阴影和遮挡物越近——硬阴影。阴影和遮挡物越远——软阴影。

有一个数学公式：$ w_{Penumbra}=(d_{Receiver}-d_{Blocker})\cdot w_{Light}/d_{Blocker}$

PCSS的完整算法
- Step 1 Blocker Search 遮挡物搜索，在一个确切距离中得到平均遮挡深度。
- Step 2 Penumbra Estimate 半影估计，用平均遮挡深度来决定filter size
- Step 3 Percentage Closer Filter 

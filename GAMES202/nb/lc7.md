# Lecture 7 Real-time Global Illumination (in 3D)
## Wavelet 小波 2D Harr Wavelet
也是一套基函数。不同函数的定义域也有所不同。小波的压缩方式和SH等其他不同，他的基函数系数有的很接近0，就可以直接当作0作为展开方式，并且可以保留全频率。这时候用的多是Cube Map。
- 优点：小波变换可以保留高频信息，属于全频滤波，比起只保留低频信号的SH更清晰，看起来效果很好。
- 缺点：不易进行旋转变换

## Global Illumination 实时渲染中的全局光照
直接光照好做，间接光照麻烦。每个直接光照的点，都应该作为间接光照的次级光源。

Reflective Shadow Map (RSM)
- 把每一个像素都作为一个次级光源
- 把Rendering Equation的 $d\omega$ 变换为 $dA$
- (看不懂了，明天再看吧)


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

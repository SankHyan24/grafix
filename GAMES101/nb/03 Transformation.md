# Transformation

## Scale Matrix

$$
\left[ \begin{matrix} y' \\ x' \end{matrix} \right]
=
\left[ \begin{matrix} s & 0 \\ 0 & s \end{matrix} \right]

\left[ \begin{matrix} y \\ x \end{matrix} \right]
$$

## Reflection Matrix

$$
\left[ \begin{matrix} y' \\ x' \end{matrix} \right]
=
\left[ \begin{matrix} -s & 0 \\ 0 & s \end{matrix} \right]
\left[ \begin{matrix} y \\ x \end{matrix} \right]
$$

## Shear Matrix

$$
\left[ \begin{matrix} y' \\ x' \end{matrix} \right]
=
\left[ \begin{matrix} 1 & a \\ 0  1 \end{matrix} \right]
\left[ \begin{matrix} y \\ x \end{matrix} \right]
$$

## Rotate Matrix
绕着中心轴线旋转
$$
\left[ \begin{matrix} y' \\ x' \end{matrix} \right]
=
\left[ \begin{matrix} cos\theta & -sin\theta \\ sin\theta & cos\theta \end{matrix} \right]
\left[ \begin{matrix} y \\ x \end{matrix} \right]
$$

## Translation Matrix

$$
\left[ \begin{matrix} y' \\ x' \end{matrix} \right]
=
\left[ \begin{matrix} a & b \\ c & d \end{matrix} \right] 
\left[ \begin{matrix} y \\ x \end{matrix} \right]
+
\left[ \begin{matrix} t_y \\ t_x \end{matrix} \right]
$$

## Homogeneous Coordinates

**Translation cannot be represented in Matrix form**  

**Add a third coordinate (w)**

- **2D point = $(x,y,1)^T$** 

- **2D vector = ${(x,y,0)}^T$**
  
  $$\left[ \begin{matrix} y' \\ x'\\1/0 \end{matrix} \right]
  =
  \left[ \begin{matrix} a & b &t_x\\ c & d & t_y\\ 0 & 0 & 1\end{matrix} \right] 
  \left[ \begin{matrix} y \\ x \\ 1/0\end{matrix} \right]$$

## Inverse Matrix

## Composing Matrix
  Multiply serveral matrices together



# 3D Transformations
## Using homogeneous coordinates again:
  - 3D point = ${(x,y,z,w)}^T,(w\neq 0)$ denotes a point${(x/w,y/w,z/w)}^T$
  - 3D vector = ${(x,y,z,0)}^T$
  - Use 4 $ \times $ 4 Matrix
$$
\left[ \begin{matrix} 
  a & b & c & t_x \\ 
  d & e & f & t_y \\
  g & h & i & t_z \\
  0 & 0 & 0 & 1 \end{matrix} \right]
$$
## 计算过程：先线性变换，再平移
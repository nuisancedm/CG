# 点乘
定义：a · b = c （a，b为向量，c是一个标量）  
在数学上c = | a || b |cosθ  
如果有 A = (Ax, Ay, Az); B = (Bx, By, Bz)  
那么$$A · B = AxBx + AyBy + AzBz  $$

点乘的性质：
* 可以快速得到两个向量的夹角：cosθ = (a · b) / | a || b |
* 可以得到一个向量再另一个向量上的投影：b在a上的投影向量 = [ (a · b) / | a | ^ 2 ] · a

点乘的应用： 
* 分解向量
* 判断两个方向有多么接近：a · b -> 1 (接近)； a · b -> -1 (远离)
* 判断两个方向的朝向： a · b > 0 (方向大致相同)；a · b < 0 (方向大致相反)；  a · b = 0 (两向量垂直)

# 叉乘
定义：a × b = c (a，b，c为向量)  
如果有 A = (Ax, Ay, Az); B = (Bx, By, Bz)  
那么有：
$$
A×B =(AyBz - AzBy, AzBx - AxBz, ByAx - BxAy) = A^{*}B = \begin{pmatrix}
0 & -a_{z} & a_{y}\\
a_{z} & 0 & -a_{x}\\
-a_{y} & a_{x} & 0
\end{pmatrix}
\times
\begin{pmatrix}
b_{x}  \\
b_{y} \\
b_{z}
\end{pmatrix}
$$

叉乘的性质：
* c向量和a和b所构成的平面垂直，其方向为右手螺旋定则 (右手四指A->B，大拇指为c的方向)，  
其长度为| A || B |sinθ，若a和b平行，则a × b = 0  

点乘的应用：
* 建立坐标系，值得注意的是unreal和unity都是左手坐标系。
* 判断左右，内外关系


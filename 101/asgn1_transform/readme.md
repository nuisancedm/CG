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
那么有:  

$$
A×B =(AyBz - AzBy, AzBx - AxBz, ByAx - BxAy) = A^{*}B =
\begin{pmatrix}
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

# MVP变换

## 模型变换 Model

安排物体在空间中的位置，姿态，包括旋转平移缩放等各种变换。

### 二维模型变换

**缩放scale**：Sx和Sy为两轴的缩放因子。

$$
\begin{pmatrix}
x'  \\
y'  \\
\end{pmatrix}
=
\begin{pmatrix}
S_{x} & 0\\
0 & S_{y} \\
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
\end{pmatrix}
$$

**翻转reflection**：例子为x轴翻转，y轴翻转同理

$$
\begin{pmatrix}
x'  \\
y'  \\
\end{pmatrix}
=
\begin{pmatrix}
-1 & 0\\
0 & 1 \\
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
\end{pmatrix}
$$

**切变shear**：样例为沿x轴向正方向切边

$$
\begin{pmatrix}
x'  \\
y'  \\
\end{pmatrix}
=
\begin{pmatrix}
1 & a\\
0 & 1 \\
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
\end{pmatrix}
$$

**旋转rotate**：样例为绕(0,0)点逆时针旋转θ度

$$
\begin{pmatrix}
x'  \\
y'  \\
\end{pmatrix}
=
\begin{pmatrix}
cosθ & -sinθ\\
sinθ & cosθ \\
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
\end{pmatrix}
$$  
  
上面的四种变换可以被统称为**线性变换（Linear Transform）**  
都是x'=Mx的形式。  

**平移transition：** 平移不属于线性变换，他无法写成x'=Mx的形式，他是一种仿射变换

$$
\begin{pmatrix}
x'  \\
y'  \\
\end{pmatrix}
=
\begin{pmatrix}
 a & b\\
c & d \\
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
\end{pmatrix}
+
\begin{pmatrix}
t_{x}  \\
t_{y}  \\
\end{pmatrix}
$$  

### 齐次坐标系

我们不希望将平移变换区别对待，我们需要一种方法来统一平移和其他线性变换，因此我们引入齐次坐标的概念。  
对于一个2D点，我们记成 [x, y, 1];  
对于一个2D向量，我们记成 [x, y, 0]。
在齐次坐标下，点和向量有以下性质，非常符合知觉。

* vec + vec = vec
* point - point = vec
* point + vec = point
* point + point = mid point

此时仿射变换可以写成：

$$
\begin{pmatrix}
x'  \\
y'  \\
w'
\end{pmatrix}
=
\begin{pmatrix}
 a & b & t_{x}\\
c & d & t_{y}\\
0 & 0 & 1
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
1
\end{pmatrix}
$$  
其中abcd为线性变换部分，txty为平移部分。
仿射变换的对点的执行顺序是**先执行线性变换部分，再执行平移部分**。

### 逆变换  

逆变换矩阵就是一个变换矩阵的逆矩阵

### 变换组合  

* 复杂的变换可以通过一系列的简单变换得到。  
* 变换的顺序是重要的，因为**矩阵的乘法不满足交换律**  
$$R_{45}\times T_{(1,0)} \neq  T_{(1,0)} \times R_{45}$$  
* 变换的应用从右往左，**变换永远应该写在坐标的左边**，下式为先旋转再平移
$$T_{(1,0)} \times R_{45} \times\begin{pmatrix}x  \\ y  \\ 1\end{pmatrix}$$  
* 矩阵有**结合律**，多次变换可以乘起来成一个矩阵来使用。

### 三维变换  

三维变换基本与二维变换的理论相同。  
三维仿射变换如下（与二维不能说一模一样，只能说）：
$$
\begin{pmatrix}
x'  \\
y'  \\
z'  \\
w'
\end{pmatrix}
=
\begin{pmatrix}
 a & b & c & t_{x}\\
d & e & f & t_{y}\\
g & h & i & t_{z}\\
0 & 0 & 0 & 1
\end{pmatrix}
\times
\begin{pmatrix}
x  \\
y  \\
z  \\
1
\end{pmatrix}
$$  

### 三维旋转

三维旋转不同于其他变换，他比较复杂，我们通常希望给出一组数据，可以得到这个物体在空间中的姿态。有三种方法可以实现。

#### 矩阵旋转

注意y轴旋转变换是的sinα和-sinα位置反了，这是因为y是由z×x得到的，一样都是逆时针旋转
$$R_{x}(α) = \begin{pmatrix}
 1 & 0 & 0 & 0\\
0 & cosα & -sinα & 0\\
0 & sinα & cosα & 0\\
0 & 0 & 0 & 1
\end{pmatrix}
$$

$$
R_{y}(α) = \begin{pmatrix}
 cosα & 0 & sinα & 0\\
0 & 1 & 0 & 0\\
-sinα & 0 & cosα & 0\\
0 & 0 & 0 & 1
\end{pmatrix}
$$

$$
R_{z}(α) = \begin{pmatrix}
 cosα &  -sinα & 0 & 0\\
sinα & cosα & 0 & 0\\
0 & 0 & 1 & 0\\
0 & 0 & 0 & 1
\end{pmatrix}
$$
相比只能绕着xyz轴旋转，还有一种围绕任意轴旋转的矩阵，叫Rodrigus旋转公式：绕n轴(n过原点)旋转α度
$$R_{(n,α)} = cosα\cdot I + (1-cosα)nn^{T} + sinα\begin{pmatrix}
 0 & -n_{z} & n_{y}\\
n_{z} & 0 & -n_{x}\\
-n_{y} & n_{x} & 0
\end{pmatrix} $$

#### 欧拉角  
欧拉角记录三个旋转角度，这三个旋转角度是三个轴的旋转变换角度。  
要用欧拉角来获得物体的最终姿态，我们不仅需要给出三个旋转角度，还需要给出旋转顺序和内外旋方式。  
先理解给定的参数：
* 欧拉角：通常由三个角度组成，分别代表绕着三个主轴（x、y、z）的旋转。
* 旋转顺序：指定了进行旋转的轴的顺序，如ZYX表示先绕z轴旋转，然后绕y轴旋转，最后绕x轴旋转。
* 内旋（局部旋转）：旋转是相对于物体自身的坐标系进行的，即随着每次旋转，物体自身坐标系也跟着旋转。
* 外旋（全局旋转）：旋转是相对于一个固定的外部或全局坐标系进行的，不管物体如何旋转，参考坐标系保持不变。  

1：**给定参数** ：(α, β, γ)；xyz； 内旋  
2：根据(α, β, γ)和xyz**构建旋转矩阵**。 其中Rx，Ry，Rz就是普通的旋转矩阵，注意y的sinθ是反的。
$$R_{内旋}=R_{x}(α)R_{y}(β)R_{z}(γ) $$
$$R_{外旋}=R_{z}(γ)R_{y}(β)R_{x}(α)$$
3：应用矩阵在点上。

万向锁问题：欧拉角经典问题，当某一个轴旋转到90度之后，会丢失一个自由度。  
如何解决万向锁问题：把不可能会转90度的轴放在中间转，或者用四元数。

#### 四元数
四元数的优点：存储空间小，没有万向锁，方便插值。  
一个四元数由一个实部和三个虚部组成
$$q=w+xi+yj+zk$$

给定一个旋转轴u=(ux, uy, uz) 和旋转角度θ 有以下四元数表示一个旋转四元数：
$$q=cos(θ/2)+(u_{x}+u_{y}+u_{k})sin(θ/2)$$

一旦有了表示旋转的四元数，可以使用它来旋转一个点或向量。给定一个点或向量v（也可以看作是一个纯虚四元数，实部为0），使用四元数q对v进行旋转：
$$v'=qvq^{-1}$$

四元数组合：假设有两个四元数q1和q2，分别表示两个旋转操作，那么这两个操作的组合(先q1转再q2转)可以表示为：
$$q_{1} = a+bi+cj+dk$$
$$q_{2} = w+xi+yj+zk$$
$$q_{3}=q_{1}\times q_{2} = (aw-bx-cy-dz)+(ax+bw+cz-dy)i+(ay-bz+cw+dx)j+(az+by-cx+dw)k$$
若q1和q2都是单位四元数，那么q3也一定是单位四元数。

四元数转换矩阵
$$q = w+xi+yj+zk$$
$$R_{q}=\begin{pmatrix}
 1-2y^2-2z^2 & 2xy-2wz & 2xz+2wy\\
2xy+2wz & 1-2x^2-2z^2 & 2yz-2wx\\
2xz-2wy & 2yz+2wx & 1-2x^2-2y^2
\end{pmatrix}$$

## 视图变换 view
视图变换安排相机在空间中的位置与朝向。
首先定义几个相机的参数：
* 位置position：
* 朝向gaze_direction
* 向上方向up_direction

视图变换view transform的作用就是对空间中任意一个相机，把它的位置重置在参考系原点，向上方向对齐y轴，看向-z方向。同时对空间中的其他所有物体也应用这个变换，这样才能保持物体和相机之间的相对位置保持不变。

View变换的本质就是一个旋转矩阵和一个平移矩阵，先平移再旋转
$$M_{view} = \begin{pmatrix}
 1-2y^2-2z^2 & 2xy-2wz & 2xz+2wy\\
2xy+2wz & 1-2x^2-2z^2 & 2yz-2wx\\
2xz-2wy & 2yz+2wx & 1-2x^2-2y^2
\end{pmatrix}$$

## 投影变换 project

将三维空间投影到二维空间

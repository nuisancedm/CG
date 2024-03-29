# 代码

``` C++
    场景中添加物体，光源，平面。
    创建 framebuffer 

    for(每个像素){
        从这个像素发射光线r
        framebuffer[m++] = castRay();
    }

    castRay(){ //该函数返回一条光线看到的颜色
        Vector3f hitColor = scene.backgroundColor;
        if(auto payload = trace(); payload){
            switch(materialType){
            case 反射并折射{
                反射castRay();
                折射castRay()；
            }
            case 反射{
                反射castRay();
            }
            default：{
                Phong模型着色
            }
        }
    }

    std::optional<hit_payload> trace(){ //该函数返回一条光线在场景中的最近相交信息
        float tNear = INF;
        初始化payload
        for(场景中的所有物体){
            float = tNearK = INF;
            uint32_t indexK;
            Vector2f uvK;
            if(intersect(tNearK, indexK, uvK) && tNearK < tNear){
                更新payload和tNear；
            }
        }
        return payload;
    }

    bool intersect(...float &tnear, unint32_t &index, Vector2f &uv){ //该函数返回bool是否和某个物体相交
        bool intersect = false;
        for(物体上的所有三角形中的第k个){
            float t, u, v;
            if(rayTriangleIntersect(...t, u, v) && t < tnear){
                tnear = t;
                uv.x = u;
                uv.y = v;
                index = k;
                intersect = true;
            }
        }
        return intersect;
    }

    bool rayTriangleIntersect(...float &tnear, float &u, float v){//该函数返回bool一条光线是否和某个三角形相交
        Moller Trumbore算法  
        计算出u，v，t，通过引用的方式返回  

        tnear = ;
        u = ;
        v = ;

        if(u,v,t,(1-u-v)>=0) return true;

        return false;
    }

```

# 以下是笔记

# Shadow map

在以往着色流程中，我们只对一个shading point进行着色，只考虑这个shading point自己的法线，光源方向，观察方向，uv坐标等属性，而不考虑场景中其他地方对这个shading point的影响。而这是不正确的，因为当这个shading point和光源之间有东西阻挡的时候，这样的做法不会正确的计算阴影。

shadow mappping是在光栅化流程下实现阴影的主要方法。  

* shadow mapping在计算阴影的时候不知道场景的几何信息。
* shadow mapping会有走样问题。
* 主要思想：如果一个点不在阴影了，那么它一定能够同**时被摄像机和光源看到**。
* 经典的shadow mapping只能处理点光源的阴影，这些阴影都是硬阴影，都有明显的阴影边界。

**shadow mappping 第一步**： 我们在点光源处放置一个相机，在这个位置进行一遍光栅化，我们就能得到光源能看到的点的深度。  
**shadow mappping 第二步**： 从真正的相机看场景，对所有真正相机看到的点的坐标**投影**回光源相机下的坐标。将投影回光源相机的坐标的深度和原本光源相机下记录的深度对比，如果两个深度一致，那么该点不是阴影点。  

总结：shadow mapping是一个两趟的过程，尽管他有很多问题，阴影图的分辨率会影响质量，且只能处理点光源硬阴影，且会受到浮点数精度影响，但是他仍然是目前主流的制作阴影的方法。

# Ray Tracing(Whitted-style)

光栅化的问题：

* 最大的问题是光栅化没法表示global effects，比如软阴影(soft shadow)，磨砂表面反射(glossy reflection)，间接光照(indirect illumination)
* 光栅化渲染速度比较快，但是效果通常较差

光线追踪是不同于光栅化的一种成像方式。
光追比较准确，但是通常来说非常慢，通常来用在离线渲染上。

# Basic Ray Tracing Algorithm

我们首先来定义光线：

* 光线沿直线传播 (实际上是错误的)
* 光线和光线不会发生碰撞 (实际上也是错误的)
* 光线从光源出发，打到场景中，经过反射折射进入我们的相机
* 光线是可逆的，光线可以从光源到达我们的眼睛，我们的眼睛也可以反向顺着光线找到光源。光追的追踪实际上就是从相机出发，最后找到一个光源。

# Ray Casting

我们从光线投射开始  
主要流程：

* 我们从相机出发，对图片平面的每一个像素发射一根光线。  
* 这个光线可能会与场景中的某个物体相交，也有可能不和任何东西相交。  
* 然后我们把交点和光源连线，如果这个点对光源也可见，那么就形成了一条有效光路，从而对像素进行着色。

Ray Casting和光栅化相同，都假设光线只反射一次，他能够得到和光栅化差不多的结果。

# Recursive(Whitted-Style) Ray Tracing

主要流程：

* 我们从相机出发，对图片平面的每一个像素发射一根光线。
* 这个光线可能会与场景中的某个物体相交，也有可能不和任何东西相交。
* 如果有交点，那么会同时发生镜面反射，折射，漫反射，这些反射的光线也有可能和其他位置的物体相交。
* 把所有交点着色求和，当做最终像素的着色。

## Ray-Surface Intersection

### Define The Ray

一条光线由光源坐标o和方向向量d所决定，下式的含义是一条光线在t时间到达了什么位置。
$$
r(t)=o+td
$$

### Define the Plane

一个平面由一个点坐标p’和一个法线N定义，对于这个平面上的其他点p，则满足以下方程 （

$$
(p-p')\cdot N =0（平面上任意两点的连线和法线垂直）
$$

### Ray Intersection With Sphere

对于任意一个在球上的点p，满足以下方程
$$
(p-c)^2-R^2=0
$$
那么测试光线与球的交点就是解下面这个关于t的一元二次方程的解(除了t其他变量全部已知)
$$
(o+td-c)^2-R^2=0
$$
如果t无实正根，那么光线不和球相交；如果t有唯一实正根，光线和球相切；如果t有两个实正根，那么光线和球相交。  

### Ray Intersection With Any Implicit Surface

从球交拓展而来，有隐式表面：
$$
f(p) = 0
$$
交点则是求解关于t的一个方程。
$$
f(o+td)=0
$$

### Ray Intersection With Triangle Mesh

普通算法：

* 我们先对光线和三角形所在平面求交点, t必须为正解。

$$
(o+td - p') \cdot N =0
$$
$$
t=\frac{(p'-o) \cdot N}{d \cdot N}
$$

* 判断交点是否在三角形内。

Moller Trumbore 算法：

* 对于三角形内任意一个点，一定可以被三角形的三个顶点坐标线性表示。
* 如果该点同时满足o+td，那么它就是光线和三角形的交点，即解下面这个方程：

$$
O+tD = (1-b_1-b_2)P_0+b_1P_1+b_2P_2
$$

解这个方程中t,b1,b2为三个未知数,**若t,b1,b2,(1-b1-b2)都大于等于0**，则相交。（其中b1，b2 (1-b1-b2)为交点的重心坐标  

$$
\begin{bmatrix}
t\\
b_1\\
b2
\end{bmatrix}
=
\frac {1}{S_1 \cdot E_1}
\begin{bmatrix}
S_2 \cdot E_2\\
S_1 \cdot S\\
S_2 \cdot D
\end{bmatrix} \\
\\
E_1=P_1-P_0\\
E_2=P_2-P_0\\
S=O-P_0\\
S_1=D \times E_2\\
S_2=S \times E_1
$$

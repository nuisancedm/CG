# Program

补充 rasterize_triangle in rasterizer.cpp；  
补充 phong_fragment_shader in main.cpp；  
补充 texture_fragment_shader in main.cpp；  
补充 bump_fragment_shader in main.cpp；  
补充 displacement_fragment_shader in main.cpp；  

本代码基于CPU比较完整的走了一遍管线。以下是代码详解：  

## main

首先读取了一个OBJ，生成了一个三角形列表TriangleList。  
三角形列表中每一个三角形类都记录了本三角形的顶点坐标向量v4f，法线向量v3f，纹理坐标v2f，对应**管线的input部分**。  
初始化光栅化器，我们需要按照我们启动程序的参数来决定，光栅化对象的材质位置和选用的shader。  
我们定义eye_pos为{0,0,10}，这是相机的位置。  

清空光栅化器的frame_buffer和depth_buffer。
为光栅化器设置MVP矩阵。
执行r.draw，开始画三角形。

## r.draw函数

r.draw画的三角形列表中的所有三角形。

for TriangleList中的每个triangle{
    对这个三角形每个顶点做MV变换。把他们从模型空间变换到观察空间。

    对顶点做MVP变换并除以W。

    对顶点法线做mv逆转置变换，对，转法线是mv逆转置。

    对顶点做视口转换，把他们从观察空间变换到屏幕空间。

    重新set三角形的所有属性。

    开始光栅化
    rasterize_triangle(newtri, viewspace_pos); newtri中的坐标是屏幕坐标，viewspace_pos是相机坐标
}

## rasterize_triangle

画一个三角形，这个函数执行总三角形数n次：
确定三角形屏幕空间包围盒。
for （包围盒内所有的像素）{
    如果这个像素在三角形内：{
        计算这个像素在三角形内重心坐标。

        深度，颜色，法线，纹理坐标，相机坐标插值。 

        获取该像素在buffer中的id。 

        如果z插值 < buffer中的深度{  
            更新深度buffer。 
             
            初始化shader payload。  
            
            运行shader获得最终颜色。
        }
    }
}

# 以下是笔记

# 着色 SHADING

**shading在美术上的定义**：用色块或者线条定义明暗和颜色。  
**shading在CG中**：给物体应用材质。且着色具有局部性，只考虑光线来的方向而不考虑光线是否被别的物体挡住，所以着色并**不会产生阴影**，shading ≠ shadow。

## Blinn-Phong Reflectance Model (布林冯反射模型)

一些重要的定义：

* **v-观测方向(viewer direction)**：以物体表面点为起点，方向指向相机的单位向量
* **n-表面法线(surface normal)**：以物体表面点为起点，垂直于表面的单位向量
* **l-光源方向(light direction)**:以物体表面点为起点,方向指向光源的单位向量
* **表面点参数(surface parameters)**：例如颜色color，光滑度shininness

### 漫反射光照 diffuse

漫反射几时光线碰到表面点之后，均匀的反射到所有方向。这意味着表面点从不同的角度看颜色是相同的。  

* **Lambert's cosine law**：同样的光以不同的角度照射到一个点上，接收到的光强度不同，光线方向l和表面法线n的夹角决定了光的强度，与cosθ = l.dot( n )正相关。
* **光的衰减**： 光线传播的过程中，越远的地方他的光线强度越低，离光源r位置处的光强 = I / r^2

结合上面两点可以得到Lambertian diffuse shading公式：

$$
L_{d}=k_{d}(I/r^2)max(0,n \cdot l)
$$  

其中Ld为表面点的最终漫反射光强度，kd为漫反射系数，I为光源强度，r为表面点到光源的距离，n为表面法线，l为光线方向。**和观测方向v完全无关**。

### 高光反射 specular

高光发生在表面比较光滑的表面上，光线的反射方向会大致接近镜面反射方向。 如果物体绝对光滑就是一个镜面，那么光线的反射方向就是镜面反射方向。当我们的观察方向和镜面反射方向大致相同的时候，就会观察到高光反射，其他时候都看不到高光。
观察方向v接近镜面放射方向 <=> **观察方向v**和**入射方向l**的**半程向量h**接近**法线n**。
$$h = bisector(v,l) = \frac{(v+l)}{||v + l||}$$  
$$L_{s} = k_{s}(I/r^2)max(0,cosα)^p = k_{s}(I/r^2)max(0,n \cdot h)^p$$

Ks为高光项系数，他通常代表了白色。 注意到公式中有一个p次方，这个p控制能看到高光的角度范围，或者说高光的大小。在布林冯模型中这个值通常是100-200，随着指数p的增加，高光大小会变小。

### 环境光照 Ambient ###  

环境光照是一个极为复杂的项，我们难以计算一个点受到了哪些环境的影响。布林冯模型对此进行了简化。  
认为环境光和法线，观察方向，光线方向都没有关系，它就是一个常数，是一个固定的颜色,他保证没有场景中没有地方时死黑的

$$
L_{a} = k_{a}I_{a}
$$

事实上要计算正确的环境光照，要学习光线追踪和全局光照相关的知识。

所以对于任何一个点，有

$$
L = L_{a}+L_{d}+L_{s} = k_{a}I_{a}+k_{d}(I/r^2)max(0,n \cdot l)+k_{s}(I/r^2)max(0,n \cdot h)^p
$$  

## 着色频率 Shading Frequencies

* 逐面着色 Flat Shading：我们认为模型的面是平整的，每个面只有一根法线。对于每个面算出一个着色结果，三角形内部没有任何的着色变化。
* 逐顶点着色 Gouraud Shading：我们求出每个顶点的法线，对每个顶点进行着色。三角形内部的颜色通过插值的方式算出来。
* 逐像素着色 Phong Shading：我们插值算出每个三角形内部每一个位置的法线，对每一个像素进行着色。

当模型面数或顶点数够密集的时候，逐面着色和逐顶点着色的效果不一定很差，同样的Phong Shading计算量也不一定大。对于面数不时很多的模型，Phong Shading的效果肯定会更好。

### 求顶点法线

任何一个顶点，肯定和不同的面有所关联。这个顶点的法线就认为是与他相连的所有的面的法线加起来按照面积求加权平均。
$$N_{v}=\frac{\sum_{i}N_i}{||\sum_{i}N_{i}||} $$

# 实时渲染管线 Graphic(Real-Time Rendering) Pipeline

管线的意思就是，从一个场景到最后的渲染图中间经过了什么过程。
这套管线已经被集成在了GPU之中。
Input：3维空间的一堆三维点  
↓  
Vertex Processing：把空间中的点**投影**到屏幕空间中  
↓  
Triangle Processing：这些屏幕空间的点连成三角形  
↓  
Rasterization：把这些三角形离散化成Fragment(在不做AA的情况下通常认为Fragment就是一个像素)  
↓  
Fragment Processing：对Fragment进行着色，决定每一个像素最后的颜色  
↓  
FrameBuffer Operation：显示像素(图片)

这些东西基本都是固定的写死的，现代GPU允许编程的部分只发生在Vertex Processing和Fragment Processing中，具体就是写shader来控制顶点和面片，控制如何着色。

## Shader  

shader是在GPU上运行，对每个像素或者每个定点都执行一次的程序。
以GLSL为例(opengl shader语言)：

``` C++
//这个fragment shader寻找了当前点位置的材质颜色，并且进行了漫反射光照计算，得到最终颜色。
uniform sampler2D myTexture; //uniform 指全局变量
uniform vec3 lightDir;
varying vec2 uv;             // 每个像素的变量 uv插值由光栅化器完成
varying vec3 norm;           

void diffuseShader(){
    vec3 kd;
    kd = texture2d(myTexture,uv);               // material color from texture
    kd *= clamp(dot(-lightDir, norm),0.0,1.0);  // Lambertian Shading model
    gl_FragColor = vec4(kd,1.0);                // ouput fragment color，gl_FragColor is a keyword in opengl
}

```

从此开始，学习到知识已经可以去学习opengl，DirectX，Vulkan等图形学API。

## GPU

这还要记笔记吗？

# 纹理映射 Texture Mapping

任何一个三维物体的表面都是二维的平面，我们把这个二维的平面称作材质空间(texture space)，所谓纹理，就是这个二维的材质空间的一张图。  
物体表面上的任何一个点和这个材质上的一个点一一对应，这就是纹理映射。  
我们在纹理图片上有一个uv坐标系，uv的范围不管分辨率和长宽比都是从0-1之间，方便处理。  

好的纹理应该有自重复性(tiled textures)。

## 重心坐标 Barycentric coordinates

重心坐标的作用是对三角形内部进行任何属性的插值(interpolation)
比如uv坐标，颜色，法线向量，都可以插值。

三角形所在平面的内部的任何一个点(x,y)，都可以认为是它三个顶点坐标(A,B,C)的**非负**线性组合:
$$(x,y) = αA+βB+γC$$  
$$α+β+γ=1 (α>=0;β>=0;γ>=0)$$
此时(α, β, γ)就是这个内部点(x, y)的重心坐标。
三角形自身的重心的重心坐标为(1/3，1/3，1/3)。
我们利用重心坐标来把三个顶点的属性进行线性的组合起来，就能得到这个点自身的属性，这就是重心坐标插值。  

重心坐标有一个问题：在经过投影变换之后，重心坐标会不一样。所以三维空间的属性要在三维空间中插值，也就是要在投影之前进行插值。

## 纹理放大 Texture Magnification

### 如果纹理太小了怎么办？

比如我们想要渲染一堵4K的墙，但是我们的材质只有1080P。  
在这种情况下，我们查询墙上一个点的UV坐标，结果有可能不是一个整数，  
这意味着可能有很多个pixel映射到了材质上的同一个texel上。
这种情况如果我们只是仅仅四舍五入成一个整数，那么许多像素会共用一个纹理颜色，就会出现锯齿状马赛克。  

解决方法是对这个点进行 **双线性插值 Bilinear interpolation**  ：  我们找到这个非整数uv坐标相邻的四个整数点。进行双线性插值得到他的颜色，双线性插值同时考虑了点自身的位置和他周围texel来决定他自身的颜色 ,也有一种Bicubic interpolation的插值法，他取周围16个点进行插值，效果更好，但是计算量更大。

### 如果纹理太大了怎么办？

比如我们想要渲染一堵1080P的墙，但是我们的纹理材质是4K的。
如果我们还是简单的对像素进行UV坐标采样，就会UV映射不连续，产生走样。

从频率的方式理解一些纹理过大产生的走样：  
走样产生的原因：采样的频率跟不上信号变化的频率。
我们的一个像素就是一次采样，而材质分辨率过高，导致一个pixel可能覆盖了材质上的许多texel，而这些texel是持续变化高频的信号。从而产生走样。

还记得我们之前是如何处理抗锯齿的吗？MSAA超采样，一个像素进行多次采样来求最终值。这里同样适用。
我们对一个像素，进行多次计算UV坐标，最后求平均，这么做没问题，效果也不错，但是性能消耗巨大。
我们不想通过超采样的方式得到平均值，有没有一种方法能输入像pixel坐标，直接得到这个pixel覆盖的texels的平均值？

#### MIPMAP (image pyramid)

mipmap允许我们进行快速的，近似的，正方形的范围查询。  
mipmap就是从一张图生成许多张低分辨率的图，每次分辨率砍一半。
mipmap增加了1/3的存储量，来记录某一个范围内的平均值。

我们怎么知道一个像素在材质上占用了一个边长L为多少的正方形区域？应该在第几层D的mipmap查？  
对于一个像素(x，y)，我们可以查询到u，v在x方向的微分(变化率)，和y方向的微分(变化率)。并求uv在xy方向上各自的纹理梯度，取最大值作为这个像素的纹理梯度L。通过L求得mipmap层级D。

$$L=max(\sqrt{(\frac{du}{dx})^2+(\frac{dv}{dx})^2}, \sqrt{(\frac{du}{dy})^2+(\frac{dv}{dy})^2})$$

$$D = log_{2}L$$
  
D不是整数怎么办？插值！  
D=1.8，先在第1层上进行双线性插值，然后在第2层上进行双线性插值，然后对两个结果进行一个单线性插值，得出1.8层的颜色。

MIPMAP也有一个问题，mipmap在远处会出现模糊(overblur)，这是因为mipmap只能查询一个正方形内的平均值，而经过透视的远景一个像素覆盖的形状可能和一个正方形相差过大。

#### 各向异性过滤 Anisotropic Filtering Ripmap

各向异性过滤和mipmap不同他同时也生成的图片沿x，y方向压扁之后的图。
各项异性过滤允许我们进行在一个长方形内进行查询平均值。但是他还是没有解决斜着的长方形的。

## 纹理应用

纹理映射伪代码：

```C++
for each rasterized screen sample (x,y):
    (u, v) = evaluate texture coordinate at (x, y) // using barycentric coordinates
    texcolor = texture.sample(u, v)
    set sample's color to texcolor // usually the diffuse albedo kd
```

在现代GPU架构中，**纹理 = 一块内存 + 这块内存上的范围查询算法**(filtering)，通用的来讲是一块数据给像素用来查询和计算。而不是仅仅局限在材质=图像上。  
从这个角度上来讲，纹理就有许多其他的应用：

**环境光照 Environment Lighting**：
我们把一个物体每个点受到的环境光全部记录下来，并且认为这些光来自无限远处，只记录方向。渲染的时候按照点的位置去查询环境光。  

* Sphere Environment Map：用一个镜面的球来记录环境光信息，并把这个球进行二维展开，这样又一个缺点，就是球的极点位置会发生扭曲。
* Cube Environment Map: 用一个正方体盒子来包裹这个镜面球，正方体的6个面和球上的各个部分一一映射，就能得到Cube Map，这样做解决了扭曲问题，但是需要一些额外的计算。

**凹凸贴图/法线贴图 Bump Map/Norm Map**：  
用纹理定义一个点的凹凸偏移量，即记录每一个点的相对高度。相对高度发生了变化，法线也会法线变化，从而影响了着色。通过法线贴图，我们扰动了表面法线，而不改变实际的几何。

* 如何计算凹凸贴图的新法线？：  
 原本p点表面法线 n(p) = (0,0,1)  （局部坐标系）  
 凹凸之后p点微分：dp/du = c1*[h(u+1) - h(u)]; dp/dv = c2*[h(v+1) - h(v)];  
 凹凸之后的p点法线 n = (-dp/du,-dp/dv, 1).normalized();  
 坐标系变换，把凹凸之后的法线变换回世界坐标系。

**位移贴图 displacement map**  
不同于凹凸贴图只换算成法线的变换，位移贴图里真正的移动了顶点位置
位移贴图要求模型面数较高。

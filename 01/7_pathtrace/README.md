# 辐射度量学

## Radiant Energy

Radiant Energy：电磁辐射的能量，通常用焦耳J作为单位来计量。很少用这个概念
$$
 Q[J=Joule]
$$

## Radiant Flux

Radiant Flux：单位时间内辐射的Radiant Energy,以瓦特或者流明位单位。

$$
\Phi \equiv \frac{dQ}{dt}[W=Watt][lm=lumen]
$$
Flux也可以被理解位power，可以被理解为一个光源的亮度。

## Radiant Intensity

Radiant Intensity：power per unit solid angle
$$
I(\omega) \equiv \frac{d\Phi}{d\omega}
$$

$$
[\frac{W}{sr}][\frac{lm}{sr}=cd=candela]
$$

$$
\Phi = \int_{S^2} I(\omega) d\omega = 4\pi I
$$

$$
I=\frac{\Phi}{4\pi}
$$

* 补充：微分立体角,他描述θ和φ变化一个很小的角度，立体角的变化速度

$$
    dA=(r\,d\theta)(rsin\theta \,d\phi) = r^2sin\theta \, d\theta d\phi
$$
$$
    d\omega = \frac{dA}{r^2} = sin\theta \, d\theta d\phi
$$

## Irradiance

Irradiance： power per unit area（面积指垂直与光线方向的有效面积）
$$
    E(x) \equiv \frac{d\Phi(x)}{dA}
$$
$$
    [\frac{W}{m^2}][\frac{lm}{m^2}=lux]
$$

## Radiance

Radiance(Luminance)：power **per unit solid angle，per projected unit area**
$$
L(p,\omega) \equiv \frac{d^2\Phi(p,\omega)}{d\omega \, dAcos\theta}
$$

单位面积在一个单位立体角的辐射power  
Radiance等价于irradiance per unit solid angle (incident Radiance)  
Radiance等价于Intensity per unit area (Exiting Radiance)

## Irradiance vs. Radiance

**Irradiance**: total power received by area **dA**  
**Radiance**: power received by area **dA** from a specific direction **dω**
$$
dE(p,\omega) = L_i(p,\omega)cos\theta d\omega
$$

$$
E(p) = \int_{H^2} L_i(p,\omega)cos\theta d\omega
$$

# Bidirectional Reflectance Distribution Function(BRDF)

BRDF represents how much light is reflected into each outgoing direction ωr from each incoming direction。  
$$
f_r(\omega_i \rightarrow \omega_r)=\frac{dL_r(\omega_r)}{dE_i(\omega_i)} = \frac{dL_(\omega_r)}{L_i(\omega_i)cos\theta_i d\omega_i} \, [\frac{1}{sr}]
$$  

BRDF是一个比率，描述了从入射方向w_i到反射方向w_r的能量转化效率。它告诉我们，从w_i方向来的光有多少比例会被反射到w_r方向上。

# The Reflection Equation

一个单位面积dA，对于一个反射方向ω_r，把所有单位立体角的irradiance乘上他们各自的BRDF系数，并全部积分起来，就是w_r方向的反射radiance
$$
L_r(p,\omega_r) = \int_{H^2} f_r(\omega_i \rightarrow \omega_r)L_i(p,\omega_i) cos\theta_i d\omega_i
$$  

* 反射方程的含义：  
反射方程描述一个表面点p在特定反射方向w_r上的Radiance，这个Radiance是通过对所有可能的入射方向wi上的irradiance进行加权积分得到的，权重由表面的BRDF和入cosθ决定。

更通俗的解释：  

* 对于空间中的一个点p(认为点p就是一个单位面积)，有一条来自ω_i方向的光线ray1打中了他。
* 那么点p接收到的来自 ω_i方向的irradiance 就是 （ray1的radiance）*  cosθ，其中θ是点p法线和ω_i的夹角。  
* 点p来自 ω_i 方向的irradiance 会全部被吸收并重新向四面八方发射出去。
* 向四面八方反射出去的光中，有一条光线以w_r方向射进了摄像机。
* 对于这一根光线rayi，摄像机接受到的irradiance为  w_r方向的BRDF *（p来自 ω_i 方向的irradiance）
* 对于所有光线都进行这样的计算并积分，就能得到正确的反射。

# The Rendering Equation

$$
    L_o(p,w_o) = L_e(p,w_o) + \int_{\Omega+}L_i(p,\omega_i)f_r(p,\omega_i,\omega_o)(n\cdot \omega_i)d\omega_i
$$

* 注意：对于所有角度，都认为他的朝向是从着色点向外，和blin-phon一样。

其中
$$
L_o(p,w_o) 未知，为最终相机看到的着色点的radiance\\
L_e(p,w_o) 已知，为着色点自发光radiance\\
L_i(p,\omega_i) 未知，为着色点接受到某一条光线的irradiance\\
f_r(p,\omega_i,\omega_o)(n\cdot \omega_i) 已知，BRDF和cos值，权重
$$

我们对这个式子进行简化
$$
    I(u) = e(u)+\int I(v)k(u,v)dv ； 其中k为权重函数，I为radiance函数，e为自发光函数
$$

我们把加权积分操作当作一个算子K，K就是一个反射操作符，再进行简化：
$$
    L=E+KL
$$

$$
(I-K)L=E \\
L=(I-K)^{-1}E\\
L=(I+K+K^2+K^3+...)E\\
L=E+KE+K^2E+K^3E...
$$
再最终结果中，E为自发光，KE为直接光照，K^2E为一次反射间接光照，K^3E为二次反射间接光照，嗯，很好理解。  
包括了自发光，直接光照，多次间接光照的结果，就是**全局光照**。

光栅化能够做的只有E和KE项，后面的项都得用光追做。

我们的终极任务，就是解出这个渲染方程的L。

# 概率论简单回顾

* 随机变量X：X可以取很多值，取值随机
* X~p(X)概率分布：随机变量取某个值的概率，概率分布之和应该为1
* 期望：不断取随机变量，求他们的平均值。
* 概率密度函数PDF：连续的罢了，期望变成积分，概率分布之和还是1

有X~p(X); Y=f(X) Y的期望为：
$$
E[Y]=E[f(X)]=\int f(x)p(x)dx
$$

# Monte Carlo Intergration

蒙特卡洛积分解决的是一个无法写出解析式的函数的定积分问题。  
* 在积分区间随机生成N个采样点xi。
* 计算每个随机点处的函数值f(xi)，并除以采样到该点的概率密度，再求平均。
$$
\int_a^bf(x)dx = \frac{1}{N}\sum_{i=1}^N \frac{f(X_i)}{p(X_i)}\,\,\,\, where\,X_i\sim p(x)
$$

* 蒙特卡洛积分样本越多，结果越精确  
* 再x轴采样，就只能再x轴上积分

# Path Tracing 路径追踪
在whitted-style ray tracing里，总是进行镜面反射，当光线遇到漫反射表面的时候，光线就会停止。  
我们的概率方程中有两个未知项，分别是我们看的shading point的radiance和shading point的irradiance。    
这个方程有两个问题：  
1：首先这是一个积分，我们要在半球上求积分。  
2：积分中存在递归。
## 问题1：蒙特卡洛解渲染方程
我们使用上面的蒙特卡洛积分来解决。  
假设我们只渲染一个着色点的直接光照，且该点没有自发光。   
我们最终看到的这个着色点的颜色就是从四面八方打到这个着色点的光和BRDF作用的和。

我们想要计算点p朝向摄像机方向的radiance（不考虑自发光）：
$$
L_o(p,w_o) = \int_{\Omega+} L_i(p,w_i)f_r(p,w_i,w_o)(n\cdot w_i)dw_i
$$

我们有蒙特卡洛积分：
$$
\int_a^bf(x)dx = \frac{1}{N} \sum_{k=1}^N\frac{f(X_{k})}{p(X_{k})}
$$

我们的f(x)就是
$$
L_i(p,w_i)f_r(p,w_i,w_o)(n\cdot w_i)
$$

我们的概率密度通常是1/2pi，是在半球上均匀采样：
$$
p(w_i)=\frac{1}{2\pi}
$$

那么渲染方程的蒙特卡洛形式：
$$
L_o(p,w_o) = \frac{1}{N} \sum_{k=1}^N\frac{L_i(p,w_i)f_r(p,w_i,w_o)(n\cdot w_i)}{p(w_i)}
$$
对应以下伪代码  
``` C++
shade(p,wo)  
    randomly choose N directions wi
    Lo = 0.0
    for each wi  
        Trace a ray r(p,wi)
        if ray r hit the light
            Lo += (1 / N) * L_i * f_r * cosine / pdf(wi);
        Else If ray r hit an object at q
            Lo += (1 / N) * shade(q, -wi) * f_r * cosine / pdf(wi);
    return Lo
```
上面这个算法有严重的问题，光线数量会指数爆炸，使计算成为不可能。  
如果我们每次只随机选择一根光线，那么指数爆炸的问题就会被解决，如下
``` C++
shade(p,wo)  
    randomly choose 1 directions wi
    Lo = 0.0
    Trace a ray r(p,wi)
    if ray r hit the light
        return L_i * f_r * cosine / pdf(wi);
    Else If ray r hit an object at q
        return shade(q, -wi) * f_r * cosine / pdf(wi);
```
这是Path tracing，如果N不等于1，就叫做分布式光线追踪。 
但是！只有一条光线，那么渲染结果的噪声巨高，我们用Ray Generation来解决。
``` C++
    Uniformly choose N sample position within the pixel
    pixel_radiance = 0.0
    For each sample in the pixel
        shoot a ray r(canPos, cam_to_sample)
        if(ray hit the scene at P)
            pixel-radiance += (1 / N) * shade(p, sample_to_cam);
    return pixel_radiance
```
## 问题2：无限递归
现实世界的光是无限弹射的，但是我们在计算机中无法模拟无限弹射。  
我们用俄罗斯轮盘赌rr的方式来解决这个问题。

我们手动设置一个概率P 
我们以P的概率射出一条光线并且返回着色结果Lo/P  
我们以1-P的概率，不射出光线，返回着色结果0  

这种方法，仍然可以通过期望得到正确的Lo
$$
E=P*(Lo/P)+(1-P)*0 = Lo
$$
我们更改代码
``` C++
shade(p,wo)  
    Manually specify a probability P_RR
    Randomly select ksi in a uniform dist. in[0,1]
    if(ksi > P_RR) return 0.0;


    Ramdonly choose 1 direction wi
    Trace a ray r(p,wi)
    if ray r hit the light
        return L_i * f_r * cosine / pdf(wi) / P_RR;
    Else If ray r hit an object at q
        return shade(q, -wi) * f_r * cosine / pdf(wi) / P_RR;
```
上述代码是一个已经确定的正确的path tracing代码，但是他不是很高效，当每个像素内的采样点比较少的时候，渲染结果噪声会很高。

## 提升效率
上述的path tracing在低采样率的情况下，噪声很高。  
原因是因为，我们随机发射的一束光线，他击中光源的概率完全靠运气，当我们在半球均匀采样的时候，有很大一部分光线不会射到光源，是无效的。 

我们最好能直接在单位光源面积dA上采样，这样发射的光线就一定能击中光源。
我们以前的渲染方程是在着色点的单位半球对立体角dw积分，而我们想对光源采样，蒙特卡罗积分就不能用。

我们找到dA和dw的关系，并把渲染方程修改成关于dA的积分。
我们直接把光源面积往单位圆上投影，投影出来的面积就是立体角。
$$
d\omega = \frac{dAcos\theta'}{||x'-x||^2}
$$

$$
L_o(p,w_o) = \int_{A}L_i(p,\omega_i)f_r(p,\omega_i,\omega_o)\frac{cos\theta cos\theta'}{||x'-x||^2}dA
$$
此时我们对光源采样，对光源积分
``` C++
shade(p,wo)  
    // Contribution from the light source.
    L_dir=0.0
    Uniformly sample the light at x' (pdf_；light = 1 / A) 
    Shoot a ray from p to x'
    if the ray is not blocked in the middle
        L_dir = L_i * f_r * cos(theta) * cos(theta_2) / |x' - p|^2 / pdf_light

    // Contribution from other reflectors
    L_indir = 0.0
    Test Russian Roulette with Probability P_RR
    Uniformly sample the hemisphere toward wi (pdf_hemi = 1/2pi)
    Trace a ray r(p, wi)
    if ray r hit a non_emitting object at q
        L_indir = shade(q,-wi) * f_r * cos(theta) / pdf_hemi / P_RR

    return L_dir + L_indir
```
到此路径追踪完，但是点光源仍然很难处理，我们通常把点光源考虑成很小的面光源。
PT是几乎100%正确的结果，可以做到照片级真实感。


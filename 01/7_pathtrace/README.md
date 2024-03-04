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

## 对于第一个问题，求积分
我们使用上面的蒙特卡洛积分来解决。  
假设我们只渲染一个着色点的直接光照，且该点没有自发光。   
我们最终看到的这个着色点的颜色就是从四面八方打到这个着色点的光和BRDF作用的和。

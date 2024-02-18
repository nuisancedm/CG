# std::function

C++11特性，可以封装任何可调用的目标，如普通函数、lambda 表达式、函数对象、以及其他函数指针。

```C++
 std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;
```

其中<Eigen::Vector3f(fragment_shader_payload)>是封装函数的签名，他的返回值是Eigen::Vector3f，并且接收fragment_shader_payload为参数。  
使用 std::function 的好处之一是它提供了很高的灵活性。你可以在程序的不同部分为 active_shader 赋予不同的函数，**只要这些函数符合相同的签名**。这使得在运行时改变程序行为变得非常容易，例如在不同的渲染阶段使用不同的着色器函数。

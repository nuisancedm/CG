#include "Light.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main() {
  Scene scene(1280, 960);  // check the class

  auto sph1 = std::make_unique<Sphere>(Vector3f(-1, 0, -12), 2);
  sph1->materialType = DIFFUSE_AND_GLOSSY;  // 球1的材质类型，球1是磨砂漫反射
  sph1->diffuseColor = Vector3f(0.6, 0.7, 0.8);  // 球1的漫反射颜色

  auto sph2 = std::make_unique<Sphere>(Vector3f(0.5, -0.5, -8), 1.5);
  sph2->ior = 1.5;  // ior为折射率
  sph2->materialType = REFLECTION_AND_REFRACTION;  // 球2的材质类型，球2是透明反射

  scene.Add(std::move(sph1));  // 向场景中添加这两个球
  scene.Add(std::move(sph2));

  Vector3f verts[4] = {{-5, -3, -6},
                       {5, -3, -6},
                       {5, -3, -16},
                       {-5, -3, -16}};                // 四个顶点的平面
  uint32_t vertIndex[6] = {0, 1, 3, 1, 2, 3};         // 顶点顺序
  Vector2f st[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};  // 四个顶点的纹理坐标
  auto mesh = std::make_unique<MeshTriangle>(verts, vertIndex, 2, st);
  mesh->materialType = DIFFUSE_AND_GLOSSY;

  scene.Add(std::move(mesh));  // 场景中添加这个平面

  scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 0.5));  // 场景中添加光源
  scene.Add(std::make_unique<Light>(Vector3f(30, 50, -12), 0.5));

  Renderer r;       // 初始化渲染器
  r.Render(scene);  // 走

  return 0;
}
#include <fstream>
#include "Vector.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include <optional>

inline float deg2rad(const float &deg)
{
    return deg * M_PI / 180.0;
}

// Compute reflection direction
Vector3f reflect(const Vector3f &I, const Vector3f &N)
{
    return I - 2 * dotProduct(I, N) * N;
}

// [comment]
// Compute refraction direction using Snell's law
//
// We need to handle with care the two possible situations:
//
//    - When the ray is inside the object
//
//    - When the ray is outside.
//
// If the ray is outside, you need to make cosi positive cosi = -N.I
//
// If the ray is inside, you need to invert the refractive indices and negate the normal N
// [/comment]
Vector3f refract(const Vector3f &I, const Vector3f &N, const float &ior)
{
    float cosi = clamp(-1, 1, dotProduct(I, N));
    float etai = 1, etat = ior;
    Vector3f n = N;
    if (cosi < 0)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}

// [comment]
// Compute Fresnel equation
//
// \param I is the incident view direction
//
// \param N is the normal at the intersection point
//
// \param ior is the material refractive index
// [/comment]
float fresnel(const Vector3f &I, const Vector3f &N, const float &ior)
{
    float cosi = clamp(-1, 1, dotProduct(I, N));
    float etai = 1, etat = ior;
    if (cosi > 0)
    {
        std::swap(etai, etat);
    }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1)
    {
        return 1;
    }
    else
    {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

// [comment]
// Returns true if the ray intersects an object, false otherwise.
//
// \param orig is the ray origin
// \param dir is the ray direction
// \param objects is the list of objects the scene contains
// \param[out] tNear contains the distance to the cloesest intersected object.
// \param[out] index stores the index of the intersect triangle if the interesected object is a mesh.
// \param[out] uv stores the u and v barycentric coordinates of the intersected point
// \param[out] *hitObject stores the pointer to the intersected object (used to retrieve material information, etc.)
// \param isShadowRay is it a shadow ray. We can return from the function sooner as soon as we have found a hit.
// [/comment]
std::optional<hit_payload> trace( // 该函数对一根光线遍历场景中所有的物体，
    const Vector3f &orig, const Vector3f &dir,
    const std::vector<std::unique_ptr<Object>> &objects)
{
    float tNear = kInfinity; // 初始化tNear为最远
    std::optional<hit_payload> payload;
    for (const auto &object : objects) // 遍历所有物体
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (object->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear) // 如果光线相交这个物体的某个三角形
        {
            payload.emplace();// 修改这条光线的payload
            payload->hit_obj = object.get();
            payload->tNear = tNearK;
            payload->index = indexK;
            payload->uv = uvK;
            tNear = tNearK;
        }
    }

    return payload;
}

// [comment]
// Implementation of the Whitted-style light transport algorithm (E [S*] (D|G) L)
//
// This function is the function that compute the color at the intersection point
// of a ray defined by a position and a direction. Note that thus function is recursive (it calls itself).
//
// If the material of the intersected object is either reflective or reflective and refractive,
// then we compute the reflection/refraction direction and cast two new rays into the scene
// by calling the castRay() function recursively. When the surface is transparent, we mix
// the reflection and refraction color using the result of the fresnel equations (it computes
// the amount of reflection and refraction depending on the surface normal, incident view direction
// and surface refractive index).
//
// If the surface is diffuse/glossy we use the Phong illumation model to compute the color
// at the intersection point.
// [/comment]
Vector3f castRay(
    const Vector3f &orig, const Vector3f &dir, const Scene &scene,
    int depth)
{
    if (depth > scene.maxDepth) // 如果超出最大深度直接返回黑色
    {
        return Vector3f(0.0, 0.0, 0.0); 
    }

    Vector3f hitColor = scene.backgroundColor; // 默认击中为背景色蓝色
    if (auto payload = trace(orig, dir, scene.get_objects()); payload) //如果有击中，payload为最近的击中点信息
    {
        Vector3f hitPoint = orig + dir * payload->tNear; //这条光线的击中点
        Vector3f N;  // 声明法线
        Vector2f st; // 声明纹理坐标
        payload->hit_obj->getSurfaceProperties(hitPoint, dir, payload->index, payload->uv, N, st); // 赋值法线和纹理坐标
        switch (payload->hit_obj->materialType)
        {
        case REFLECTION_AND_REFRACTION: // 如果材质信息为折射并反射
        {
            Vector3f reflectionDirection = normalize(reflect(dir, N)); // 计算反射方向
            Vector3f refractionDirection = normalize(refract(dir, N, payload->hit_obj->ior)); // 计算折射方向
            Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ? hitPoint - N * scene.epsilon : hitPoint + N * scene.epsilon; //
            Vector3f refractionRayOrig = (dotProduct(refractionDirection, N) < 0) ? hitPoint - N * scene.epsilon : hitPoint + N * scene.epsilon;
            Vector3f reflectionColor = castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1); // 反射折射分别递归再发射两条光线
            Vector3f refractionColor = castRay(refractionRayOrig, refractionDirection, scene, depth + 1);
            float kr = fresnel(dir, N, payload->hit_obj->ior);
            hitColor = reflectionColor * kr + refractionColor * (1 - kr);
            break;
        }
        case REFLECTION:
        {
            float kr = fresnel(dir, N, payload->hit_obj->ior);
            Vector3f reflectionDirection = reflect(dir, N);
            Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ? hitPoint + N * scene.epsilon : hitPoint - N * scene.epsilon;
            hitColor = castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1) * kr;
            break;
        }
        default:
        {
            // [comment]
            // We use the Phong illumation model int the default case. The phong model
            // is composed of a diffuse and a specular reflection component.
            // [/comment]
            Vector3f lightAmt = 0, specularColor = 0;
            Vector3f shadowPointOrig = (dotProduct(dir, N) < 0) ? hitPoint + N * scene.epsilon : hitPoint - N * scene.epsilon;
            // [comment]
            // Loop over all lights in the scene and sum their contribution up
            // We also apply the lambert cosine law
            // [/comment]
            for (auto &light : scene.get_lights())
            {
                Vector3f lightDir = light->position - hitPoint;
                // square of the distance between hitPoint and the light
                float lightDistance2 = dotProduct(lightDir, lightDir);
                lightDir = normalize(lightDir);
                float LdotN = std::max(0.f, dotProduct(lightDir, N));
                // is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
                auto shadow_res = trace(shadowPointOrig, lightDir, scene.get_objects());
                bool inShadow = shadow_res && (shadow_res->tNear * shadow_res->tNear < lightDistance2);

                lightAmt += inShadow ? 0 : light->intensity * LdotN;
                Vector3f reflectionDirection = reflect(-lightDir, N);

                specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)),
                                      payload->hit_obj->specularExponent) *
                                 light->intensity;
            }

            hitColor = lightAmt * payload->hit_obj->evalDiffuseColor(st) * payload->hit_obj->Kd + specularColor * payload->hit_obj->Ks;
            break;
        }
        }
    }

    return hitColor;
}

// [comment]
// The main render function. This where we iterate over all pixels in the image, generate
// primary rays and cast these rays into the scene. The content of the framebuffer is
// saved to a file.
// [/comment]
void Renderer::Render(const Scene &scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height); // 创建帧缓冲区

    float scale = std::tan(deg2rad(scene.fov * 0.5f));          // scale = tan(1/2*fov) = top/abs(near)
    float imageAspectRatio = scene.width / (float)scene.height; // imageAspectRatio = 宽高比 = width/height

    // Use this variable as the eye position to start your rays.
    Vector3f eye_pos(0); // 相机位置(0，0，0)
    int m = 0;           // 帧缓冲区内的编号
    for (int j = 0; j < scene.height; ++j) // 遍历所有像素，从这些像素发射光线
    {
        for (int i = 0; i < scene.width; ++i)
        {   
            // TODO: Find the x and y positions of the current pixel to get the direction
            // vector that passes through it.
            // Also, don't forget to multiply both of them with the variable *scale*, and
            // x (horizontal) variable with the *imageAspectRatio*

            // generate primary ray direction
            // scale = tan(1/2*fov) = top/abs(near)
            // imageAspectRatio = 宽高比 = width/height
            // top = scale;
            // right = imageAspectRatio * top;
            float x = ((float(i) + 0.5f - (scene.width / 2)) / scene.width)* 2 * imageAspectRatio * scale;
            float y = -((float(j) + 0.5f - (scene.height / 2)) / scene.height) * 2 * scale;
            // 整理一下为什么x，y是这么求的。
            // 光栅化空间下，以一个像素为一个单位，坐标系原点在屏幕左上角。
            // 世界空间下，坐标系原点在相机位置，图像平面垂直于z轴，且z轴穿过图像平面中心。
            // 我们要做的就是把光栅空间坐标[i,j]转化成世界坐标(x,y),也就是要找到当前像素在世界坐标系下的坐标。
            // float(i) + 0.5f 为找到当前像素的中心的x坐标(光栅坐标系)
            // float(i) + 0.5f - (scene.width / 2) 为 当前像素的中心的x坐标 减去 图像平面中点的x坐标，得到像素相对于图像平面中点的坐标。
            // (float(i) + 0.5f - (scene.width / 2)) / scene.width 为 当前像素在总长度中的几分之几的位置。
            // ((float(i) + 0.5f - (scene.width / 2)) / scene.width)* 2 * imageAspectRatio * scale; 然后再乘两倍的right，就得到世界坐标系下的x值
            
            Vector3f dir = normalize(Vector3f(x, y, -1)); // 这是primary ray的方向向量，需要归一化。
            framebuffer[m++] = castRay(eye_pos, dir, scene, 0); // 发射这根光线
        }
        UpdateProgress(j / (float)scene.height);
    }

    // save framebuffer to file
    FILE *fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i)
    {
        static unsigned char color[3];
        color[0] = (char)(255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (char)(255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (char)(255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}

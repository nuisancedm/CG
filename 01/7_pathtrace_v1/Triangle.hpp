#pragma once
#include "Object.hpp"
#include "Material.hpp"
#include "OBJ_Loader.hpp"
#include <cassert>
class Triangle : public Object
{
public:
    Vector3f v0, v1, v2; //@@ vertex A B C, counter-clock wise
    Vector3f e1, e2;     //@@ 2 edges v1-v0,v2-v0;
    Vector3f t0, t1, t2; //@@ texture coords
    Vector3f normal;
    float area;
    Material *m;

    Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material *_m = nullptr)
        : v0(_v0), v1(_v1), v2(_v2), m(_m)
    {
        e1 = v1 - v0;
        e2 = v2 - v0;
        normal = normalize(crossProduct(e1, e2));
        area = crossProduct(e1, e2).norm() * 0.5f; //@@ good way to get the triangle area
    };

    bool intersect(const Ray &ray) override;
    bool intersect(const Ray &ray, float &tnear,
                   uint32_t &index) const override;
    Intersection getIntersection(Ray ray) override;
    void getSurfaceProperties(const Vector3f &P, const Vector3f &I,
                              const uint32_t &index, const Vector2f &uv,
                              Vector3f &N, Vector2f &st) const override
    {
        N = normal;
        //        throw std::runtime_error("triangle::getSurfaceProperties not
        //        implemented.");
    }
    Vector3f evalDiffuseColor(const Vector2f &) const override;
    Bounds3 getBounds() override;
    void Sample(Intersection &pos, float &pdf){
        float x = std::sqrt(get_random_float()), y = get_random_float();
        pos.coords = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
        pos.normal = this->normal;
        pdf = 1.0f / area;
    }
    float getArea(){
        return area;
    }
    bool hasEmit(){
        return m->hasEmission();
    }
};

inline bool Triangle::intersect(const Ray &ray) {return true;}
inline bool Triangle::intersect(const Ray& ray, float &tnear,
                   uint32_t &index)const {return false;}
inline Bounds3 Triangle::getBounds(){
    return Union(Bounds3(v0, v1), v2);
}

class MeshTriangle : public Object
{
public:
    float area;
    Material *m;
    MeshTriangle(const std::string &filename, Material *mt = new Material())
    {
        objl::Loader loader;
        loader.LoadFile(filename);
        area = 0;
        m = mt;
        assert(loader.LoadedMeshes.size() == 1);
        auto mesh = loader.LoadedMeshes[0];
    }
};
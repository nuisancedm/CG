#pragma once
#include "Vector.hpp"
#include "global.hpp"

enum MaterialType
{
    DIFFUSE
};

class Material
{
private:
public:
    MaterialType m_type;
    Vector3f m_emission;
    float ior;
    Vector3f Kd, Ks;
    //@@ Diffuse Reflectivity and Specular Reflectivity in bllin-phone model
    inline Material(MaterialType t = DIFFUSE, Vector3f e = Vector3f(0.0f, 0.0f, 0.0f)){}; //@@ inline construct
    inline MaterialType getType() { return m_type; }
    inline Vector3f getEmission() { return m_emission; }
    inline bool hasEmission();
};

bool Material::hasEmission()
{
    if (m_emission.norm() > EPSILON)
        return true;
    return false;
}
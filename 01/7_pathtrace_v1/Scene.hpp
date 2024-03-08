#pragma once
#include "Vector.hpp"

class Scene
{
public:
    int width = 1280;
    int height = 960;
    double fov = 40;
    Vector3f backgroundColor = Vector3f(0.235294, 0.67451, 0.843137);
    int maxDepth = 1;
    float RuassianRoulette = 0.8;

    Scene(int w, int h) : width(w), height(h)
    {
        
    }


};
#pragma once
#include "Vector.hpp"

struct Ray
{
    //@@ destination = origin+t*direction;
    Vector3f origin;
    Vector3f direction, direction_inv; //@@ these two have to be unit vector
    double t;                          //@@ transportation time,
    double tmin, tmax;

    Ray(const Vector3f &ori, const Vector3f &dir, const double _t = 0.0) : origin(origin), direction(dir), t(_t)
    {
        direction_inv = Vector3f(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
        tmin = 0;
        tmax = std::numeric_limits<double>::max();
    }

    Vector3f operator()(double t) const { return origin + t * direction; };

    friend std::ostream &operator<<(std::ostream &os, const Ray &r)
    {
        os << "[origin:=" << r.origin << ", direction=" << r.direction << ", time=" << r.t << "]\n";
        return os;
    }
};
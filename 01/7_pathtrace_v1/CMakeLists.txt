cmake_minimum_required(VERSION 3.10)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)

set(RT_SOURCES 
        main.cpp
        Object.hpp
        Vector.cpp
        Vector.hpp
        global.hpp
        Triangle.hpp
        Scene.hpp
        global.hpp 
        Triangle.hpp
        Bounds3.hpp 
        Ray.hpp
        Material.hpp
        Intersection.hpp
)

add_executable(RayTracing ${RT_SOURCES})

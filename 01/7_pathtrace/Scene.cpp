//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    //@@ BVHAccel(object list, number of primitives each leaf node has, splitMethod)
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    //@@ get all area of all lighting material.
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    //@@ 
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}
// Implement Path Tracing Algorithm here
// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{ 
    
    Vector3f L_dir; //@@ direct light
    Vector3f L_indir;//@@ indirect light

    //@@ return the intersection of the ray with the whole scene.
    Intersection obj_inter = intersect(ray);
    //@@ if this ray doesn't intersect with anything, return L_dir of 000
    if(!obj_inter.happened)
        return L_dir;
    
    //@@ if the ray hit the light directly. return the emmision
    if(obj_inter.m->hasEmission())
        return obj_inter.m->getEmission();
    
    //@@ if the ray hit some object, start from here.
    Vector3f p = obj_inter.coords; //@@ hit point position
    Material* m = obj_inter.m; //@@ hit point material
    Vector3f N = obj_inter.normal.normalized(); //@@ hit point normal.
    Vector3f wo = ray.direction; //@@ ray direction, to point.
    
    float pdf_L = 1.0; 
    Intersection light_inter ;
    //@@ sample the light area. and get the intersection.
    sampleLight(light_inter,pdf_L); 
    
    Vector3f x = light_inter.coords;
    Vector3f ws = (x - p).normalized(); //@@ point to light
    Vector3f NN = light_inter.normal.normalized();  //@@ light normal
    Vector3f emit = light_inter.emit; //@@ light emimision
    float d = (x-p).norm(); //@@ distance from point to light
    
    //@@ a new ray Obj2Light from point to light
    Ray Obj2Light(p,ws);
    //@@ calculate the distance from the new light to the scene intersection
    float d2 = intersect(Obj2Light).distance;
    
    //@@ only the ray Obj2Light is not blocked by other objects,
    if(d2-d > -0.001){
        Vector3f eval = m->eval(wo,ws,N);//@@ BRDF
        float cos_theta = dotProduct(N,ws); //@@ cos theta(point)
        float cos_theta_x = dotProduct(NN,-ws);//@@ cos theta(light)
        //@@ calculate the direct lighting of this point.
        L_dir = emit * eval * cos_theta * cos_theta_x / std::pow(d,2) / pdf_L;
    }
    
    
    float P_RR = get_random_float();
    //@@ randomly reflect a new light.
    if(P_RR<RussianRoulette){ //@@ survived in the RussianRoulette
        Vector3f wi = m->sample(wo,N).normalized(); 
        Ray r(p,wi); 
        Intersection inter = intersect(r); 
        
        if(inter.happened && !inter.m->hasEmission()){
            Vector3f eval = m->eval(wo,wi,N);
            float pdf_O = m->pdf(wo,wi,N);
            float cos_theta = dotProduct(wi,N);
            L_indir = castRay(r, depth+1) * eval * cos_theta/ pdf_O / RussianRoulette;
        }
    }
    
    return L_dir + L_indir;

}
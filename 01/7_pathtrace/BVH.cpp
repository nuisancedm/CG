#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{   
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;
    //@@ recursive build the BVH tree and return the root node.
    root = recursiveBuild(primitives);

    //@@ print the time used
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    //@@ Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    //@@ if only one object in the object list. it should be a leaf node. 
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        //@@ it is the area of all triangles of the object.
        node->area = objects[0]-> getArea(); 
        return node;
    }

    //@@ if there is two object in the object list, should be seperate in the leaf nodes.
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});
        //@@ calculate the bounding box of this two nodes.
        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
        return node;
    }
    //@@ more than 2 objects in the nodes.
    else {
        //@@ centroidBounds saves center position of each object's bounds.
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        //@@ find which axis is the longest
        int dim = centroidBounds.maxExtent();

        //@@ sort by center position
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Vector3f invDir{1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z};
    std::array<int, 3> dirIsNeg{int(ray.direction.x < 0), int(ray.direction.y < 0), int(ray.direction.z < 0)};

    //@@ if the ray doesn't intersect with the current node.
    if (node->bounds.IntersectP(ray, invDir, dirIsNeg) == false)
    {
        return Intersection(); //@@ return a empty intersection
    }

    //@@ if the ray intersect with this node and it is a leaf node.
    if (node->left == nullptr && node->right == nullptr)
    {
        //@@ test intersection with all objects of this node;
        Intersection intersec = node->object->getIntersection(ray);
        return intersec;
    }

    Intersection hit1;
    Intersection hit2;
    if (node->left != nullptr)
    {
        hit1 = getIntersection(node->left, ray);
    }
    if (node->right != nullptr)
    {
        hit2 = getIntersection(node->right, ray);
    }

    return hit1.distance < hit2.distance ? hit1 : hit2;
}


void BVHAccel::getSample(BVHBuildNode* node, float p, Intersection &pos, float &pdf){
    if(node->left == nullptr || node->right == nullptr){
        node->object->Sample(pos, pdf);
        pdf *= node->area;
        return;
    }
    if(p < node->left->area) getSample(node->left, p, pos, pdf);
    else getSample(node->right, p - node->left->area, pos, pdf);
}

void BVHAccel::Sample(Intersection &pos, float &pdf){
    float p = std::sqrt(get_random_float()) * root->area;
    getSample(root, p, pos, pdf);
    pdf /= root->area;
}
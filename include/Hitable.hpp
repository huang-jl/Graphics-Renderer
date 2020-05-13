#ifndef HITABLE_HPP
#define HITABLE_HPP

#include "AABB.hpp"
#include "Ray.hpp"

class Material;
struct Hit
{
    float t;
    Vector3f p;
    Vector3f normal; // unit
    float u, v; //击中点的u,v信息
    shared_ptr<Material> material_p;
};

class Hitable
{
  public:
    // rec参数用来记录交点的信息
    Hitable() {}
    Hitable(shared_ptr<Material> m) : material_p(m) {}
    virtual ~Hitable() {}
    //求交函数，返回值表示是否相交
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const = 0;
    //计算包围盒的函数，返回值表示该物体是否有包围盒
    virtual bool bounding_box(float t0, float t1, AABB &box) const = 0;
    /*data*/
    shared_ptr<Material> material_p;
};

#endif
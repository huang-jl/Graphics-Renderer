#ifndef MEDIA_HPP
#define MEDIA_HPP
#include "Hitable.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include <cfloat>
#include <iostream>
/****************************************
 * 体积体
 * 即一种类似烟雾、水汽的物体
 * 这里采用简化的方法，设置体积体内部为一个
 * 随机平面，光线可能与之相交，可能穿过它

****************************************/

/****************************************
 * 对于一个固定密度体积体而言，我们只需要
 * density和boundary
 * 为了方便，这里直接使用Hitable作为boundary
 * 只适用于光线和体积体至多两个交点的情况
 ****************************************/
class ConstantMedium : public Hitable
{
  public:
    ConstantMedium(shared_ptr<Hitable> o, float d, shared_ptr<Texture> t);

    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;

    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    /*data*/
    shared_ptr<Hitable> boundary;
    float density;
};

#endif
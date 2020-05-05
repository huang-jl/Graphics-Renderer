#ifndef BVH_HPP
#define BVH_HPP
#include "Hitable.hpp"
#include "AABB.hpp"
#include "Ray.hpp"
/*
 * 层次包围体BHV：用AABB实现
 * 这里认为层次包围盒也是一个Hitable
 * 其作用是一个容器，能够高效处理光线和物体是否相交
 */

class BHVNode : public Hitable
{
  public:
    BHVNode() {}
    BHVNode(Hitable **l, int n, float time0, float time1);
    virtual bool bounding_box(float t0, float t1, AABB &output_box) const override;
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;

    /*data*/
    Hitable *left_c;  //左孩子，为了通用性，指针为Hitbale
    Hitable *right_c; //右孩子
    AABB box;
};
#endif
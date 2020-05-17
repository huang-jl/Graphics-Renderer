#ifndef AABB_HPP
#define AABB_HPP
#include "Ray.hpp"
#include "Utils.hpp"

/* Parallelepiped Axis-Align Bounding Boxes
 * 为了对求交进行加速
 * 实现层次包围盒的方法
 */
class AABB
{
  public:
    AABB() {}
    AABB(const Vector3f &a, const Vector3f &b) : min_(a), max_(b) {}
    Vector3f min() const { return min_; }
    Vector3f max() const { return max_; }

    /* 判断光线r是否和包围盒相交的算法
     * 具体来说，采用slab的方式
     * 对每个轴的求光线与包围盒的相交区域
     * 最后比较这些区域是否有重叠
     */
    bool hit(const Ray &r, float tmin, float tmax, float *t = nullptr) const;

    /*data*/
    Vector3f min_; //包围盒的左下角点
    Vector3f max_; //包围盒的右下角点
};

AABB surrounding_box(AABB &box0, AABB &box1);
#endif
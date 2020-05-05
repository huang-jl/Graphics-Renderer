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
    AABB(const vec3 &a, const vec3 &b) : min_(a), max_(b) {}
    vec3 min() const { return min_; }
    vec3 max() const { return max_; }

    /* 判断光线r是否和包围盒相交的算法
     * 具体来说，采用slab的方式
     * 对每个轴的求光线与包围盒的相交区域
     * 最后比较这些区域是否有重叠
     */
    bool hit(const Ray &r, float tmin, float tmax) const
    {
        for (int axis = 0; axis < 3; ++axis)
        {
            float inv_dir = 1.0 / r.direction()[axis];
            //和包围盒的两个交点的t值(光线为P(t) = o + dir * t)
            float t0 = (min()[axis] - r.origin()[axis]) * inv_dir;
            float t1 = (max()[axis] - r.origin()[axis]) * inv_dir;
            //如果是负向的光线，那么交换t0和t1的顺序
            if (inv_dir < 0.0)
                std::swap(t0, t1);
            //每次都将t的范围缩小到slab中
            //如果出现slab的完全错开，表示和AABB没有交点，返回失败
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmin >= tmax)
                return false;
        }
        return true;
    }

    /*data*/
    vec3 min_; //包围盒的左下角点
    vec3 max_; //包围盒的右下角点
};

inline AABB surrounding_box(AABB &box0, AABB &box1)
{
    vec3 lower_left = vec3(ffmin(box0.min()[0], box1.min()[0]), ffmin(box0.min()[1], box1.min()[1]),
                           ffmin(box0.min()[2], box1.min()[2]));
    vec3 upper_right = vec3(ffmax(box0.max()[0], box1.max()[0]), ffmax(box0.max()[1], box1.max()[1]),
                            ffmax(box0.max()[2], box1.max()[2]));
    return AABB(lower_left, upper_right);
}
#endif
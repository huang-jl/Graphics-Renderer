#include "AABB.hpp"

bool AABB::hit(const Ray &r, float tmin, float tmax, float *t) const
{
    float hit_t = __FLT_MAX__;
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
        // tmin = t0 > tmin ? t0 : tmin;
        // tmax = t1 < tmax ? t1 : tmax;
        tmin = ffmax(t0, tmin);
        tmax = ffmin(t1, tmax);
        hit_t = ffmin(t0, hit_t);
        if (tmin >= tmax)
            return false;
    }
    if (t != nullptr)
        *t = hit_t;
    return true;
}

AABB surrounding_box(AABB &box0, AABB &box1)
{
    Vector3f lower_left = Vector3f(ffmin(box0.min()[0], box1.min()[0]), ffmin(box0.min()[1], box1.min()[1]),
                                   ffmin(box0.min()[2], box1.min()[2]));
    Vector3f upper_right = Vector3f(ffmax(box0.max()[0], box1.max()[0]), ffmax(box0.max()[1], box1.max()[1]),
                                    ffmax(box0.max()[2], box1.max()[2]));
    return AABB(lower_left, upper_right);
}

#ifndef SPHERE_HPP
#define SPHERE_HPP
#include "Hitable.hpp"
#include <cstdio>

/*
 *注意如果电解质球体的半径radius < 0
 *交点处法向量的方向会指向球心，因此可以用作一个气泡
 */
class Sphere : public Hitable
{
  public:
    Sphere() {}
    Sphere(Vector3f cen, float r, shared_ptr<Material> m) : Hitable(m), center(cen), radius(r) {}
    virtual bool hit(const Ray &r, float tmin, float tmax, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    /*data*/
    Vector3f center;
    float radius;
};

/*
 * 为了支持运动模糊，我们需要一个能“运动”的物体
 * 为了方便，这里采用球体
 */
class MovingSphere : public Hitable
{
  public:
    MovingSphere() {}
    /* 有两个球心位置cen0,cen1，对应t0,t1时刻
     * 然后球的运动在两个时间点之间线性插值
     * 注意球在两个时刻之外仍然有运动，只是声明t0,t1用于计算
     * 因此不必和Camera中的快门开关时间匹配
     */
    MovingSphere(Vector3f cen0, Vector3f cen1, float t0, float t1, float r, shared_ptr<Material> m)
        : Hitable(m), center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r){};

    Vector3f get_center(float ti) const { return center0 + (center1 - center0) * (ti - time0) / (time1 - time0); }
    virtual bool hit(const Ray &r, float tmin, float tmax, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    /*data*/
    Vector3f center0;
    Vector3f center1;
    float time0, time1;
    float radius;
};

#endif
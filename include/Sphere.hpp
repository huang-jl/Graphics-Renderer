#ifndef SPHERE_HPP
#define SPHERE_HPP
#include "Hitable.hpp"
#include "Utils.hpp"
#include <cstdio>

/*
 *注意如果电解质球体的半径radius < 0
 *交点处法向量的方向会指向球心，因此可以用作一个气泡
 */
class Sphere : public Hitable
{
  public:
    Sphere() {}
    Sphere(vec3 cen, float r, Material *m) : Hitable(m), center(cen), radius(r) {}
    inline virtual bool hit(const Ray &r, float tmin, float tmax, Hit &rec) const override;
    inline virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    /*data*/
    vec3 center;
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
    MovingSphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, Material *m)
        : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), material_p(m){};

    vec3 get_center(float ti) const { return center0 + (center1 - center0) * (ti - time0) / (time1 - time0); }
    inline virtual bool hit(const Ray &r, float tmin, float tmax, Hit &rec) const override;
    inline virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    /*data*/
    vec3 center0;
    vec3 center1;
    float time0, time1;
    float radius;
    Material *material_p;
};

inline bool Sphere::hit(const Ray &r, float tmin, float tmax, Hit &rec) const
{
    //参略一些可以约去的2
    vec3 oc = r.origin() - center; //球心到光线原点的向量
    float a = dot(r.direction(), r.direction());
    float b = dot(r.direction(), oc); // assert(b<0)
    float c = dot(oc, oc) - radius * radius;
    float delta = b * b - a * c;

    if (delta > 0)
    {
        float temp = (-b - sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            return true;
        }
        temp = (-b + sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            return true;
        }
    }
    return false;
}

inline bool MovingSphere::hit(const Ray &r, float tmin, float tmax, Hit &rec) const
{
    //获得光线所在时刻球体的位置
    vec3 center = get_center(r.time());

    vec3 oc = r.origin() - center; //球心到光线原点的向量
    float a = dot(r.direction(), r.direction());
    float b = dot(r.direction(), oc); // assert(b<0)
    float c = dot(oc, oc) - radius * radius;
    float delta = b * b - a * c;

    if (delta > 0)
    {
        float temp = (-b - sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            return true;
        }
        temp = (-b + sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            return true;
        }
    }
    return false;
}

/*
 * 计算包围盒的函数
 * t0,t1表示两个时间
 * 向box中填入包围盒的左下角和右上角点
 * 返回是否有包围盒
 */
inline bool Sphere::bounding_box(float t0, float t1, AABB &box) const
{
    box = AABB(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

/*
 * 计算包围盒的函数
 * t0,t1表示两个时间
 * 向box中填入包围盒的左下角和右上角点
 * 返回是否有包围盒
 *
 * 同时因为是运动的球体，我们同时也需要计算出
 * 能够包围住两个时间点形成的球体的大包围盒
 */
inline bool MovingSphere::bounding_box(float t0, float t1, AABB &box) const
{
    //已知两个包围盒，求大包围盒的lambda表达式

    AABB box0 = AABB(center0 - vec3(radius, radius, radius), center0 + vec3(radius, radius, radius));
    AABB box1 = AABB(center1 - vec3(radius, radius, radius), center1 + vec3(radius, radius, radius));
    box = surrounding_box(box0, box1);
    return true;
}

#endif
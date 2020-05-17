#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP
#include "Hitable.hpp"
#include <cfloat>

/****************************************
 * 这实际上是一个包装器
 * 需要一个Hitable的指针作为待变换的对象
 *
 * 这里并不是变换物体，而是变换光线，求出变换后
 * 的光线和物体的交点
 * 最后再反变换回去得到物体变换的效果
 ****************************************/
class Translate : public Hitable
{
  public:
    Translate() {}
    Translate(shared_ptr<Hitable> o, const Vector3f &displacement);
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    /*data*/
    shared_ptr<Hitable> object;
    Vector3f offset;
};

/****************************************
 * 表示逆时针旋转的一个包装器
 * 构造函数的参数是旋转的角度（角度制）和轴
 *      0——x轴，1——y轴，2——z轴
 ****************************************/
class Rotate : public Hitable
{
  public:
    Rotate() {}
    Rotate(shared_ptr<Hitable> o, float rotate_degree, int rotate_axis);
    Vector3f axial_rotate(const Vector3f &v) const;
    //绕着轴反向旋转
    Vector3f axial_rotate_reverse(const Vector3f &v) const;

    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;

    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    /*data*/
    shared_ptr<Hitable> object;

    int axis;
    float theta;
    float cos_theta;
    float sin_theta;
    AABB b_box;
    bool has_box;
};

#endif

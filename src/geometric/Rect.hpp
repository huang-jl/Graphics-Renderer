#ifndef RECT_HPP
#define RECT_HPP

#include "Hitable.hpp"
#include "Material.hpp"
#include "HitableList.hpp"

/****************************************
 * 和x轴、y轴平行的正方形面片
 ****************************************/
class XYRect : public Hitable
{
  public:
    XYRect() {}
    XYRect(float x_0, float x_1, float y_0, float y_1, float z_, shared_ptr<Material> m_p);
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    /*data*/
    float x0, x1, y0, y1, z;
};

/****************************************
 * 和y轴、z轴平行的正方形面片
 ****************************************/
class YZRect : public Hitable
{
  public:
    YZRect() {}
    YZRect(float y_0, float y_1, float z_0, float z_1, float x_, shared_ptr<Material> m_p);
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    /*data*/
    float y0, y1, z0, z1, x;
};

/****************************************
 * 和x轴、z轴平行的正方形面片
 ****************************************/
class XZRect : public Hitable
{
  public:
    XZRect() {}
    XZRect(float x_0, float x_1, float z_0, float z_1, float y_, shared_ptr<Material> m_p);
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    virtual float pdf_value(const Vector3f&o, const Vector3f&dir)const override;
    virtual Vector3f random(const Vector3f&o)const override;

    /*data*/
    float x0, x1, z0, z1, y;
};

class Box : public Hitable
{
  public:
    Box() {}
    Box(Vector3f left_bottom, Vector3f right_top, shared_ptr<Material> m_p);
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;

    /*data*/
    Vector3f left_b;   //左下角点
    Vector3f right_t;  //右上角点
    HitableList sides; //一个长方体的六个面
};
#endif
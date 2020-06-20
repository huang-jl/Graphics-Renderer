#ifndef HITABLE_HPP
#define HITABLE_HPP

#include "AABB.hpp"
#include "Ray.hpp"

class Material;
struct Hit
{
    float t;
    Vector3f p;
    Vector3f normal;    // unit
    float u = 0, v = 0; //击中点的u,v信息
    shared_ptr<Material> material_p;
    bool front_face; //是否是从外向内入射

    void set_normal(const Ray &r_in, const Vector3f &outward_normal)
    {
        front_face = (Vector3f::dot(r_in.direction(), outward_normal) < 0);
        normal = front_face ? outward_normal : -outward_normal;
    }
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
    //对起点o和随机方向dir，返回其对应的pdf值
    virtual float pdf_value(const Vector3f &o, const Vector3f &dir) const { return 0.0; }
    //产生一个随机方向，表示散射的光线方向，起点为o
    virtual Vector3f random(const Vector3f &o) const { return Vector3f(1, 0, 0); }
    /*data*/
    shared_ptr<Material> material_p;
};

/****************************************
 * 针对平面使用，反转平面方向
 ****************************************/
class FlipFace : public Hitable
{
  public:
    FlipFace(shared_ptr<Hitable> o) : Hitable(o->material_p), hitable(o) {}
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const
    {
        if (hitable->hit(r, t_min, t_max, rec))
        {
            rec.front_face = !rec.front_face;
            return true;
        }
        else
            return false;
    }
    virtual bool bounding_box(float t0, float t1, AABB &box) const { return hitable->bounding_box(t0, t1, box); }
    //对起点o和随机方向dir，返回其对应的pdf值
    virtual float pdf_value(const Vector3f &o, const Vector3f &dir) const override
    {
        return hitable->pdf_value(o, dir);
    }
    //产生一个随机点，用于直接采样，起点为o，返回一个光线方向
    virtual Vector3f random(const Vector3f &o) const override { return hitable->random(o); }

    /*data*/
    shared_ptr<Hitable> hitable;
};

#endif
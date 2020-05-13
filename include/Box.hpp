#ifndef BOX_HPP
#define BOX_HPP

#include "Hitable.hpp"
#include "HitableList.hpp"
#include "Rect.hpp"
class Box : public Hitable
{
  public:
    Box() {}
    Box(Vector3f left_bottom, Vector3f right_top, shared_ptr<Material> m_p)
        : Hitable(m_p), left_b(left_bottom), right_t(right_top)
    {
        sides.add_hitable(make_shared<XYRect>(left_b.x(), right_t.x(), left_b.y(), right_t.y(), left_b.z(), m_p));
        sides.add_hitable(make_shared<XYRect>(left_b.x(), right_t.x(), left_b.y(), right_t.y(), right_t.z(), m_p));
        sides.add_hitable(make_shared<YZRect>(left_b.y(), right_t.y(), left_b.z(), right_t.z(), left_b.x(), m_p));
        sides.add_hitable(make_shared<YZRect>(left_b.y(), right_t.y(), left_b.z(), right_t.z(), right_t.y(), m_p));
        sides.add_hitable(make_shared<XZRect>(left_b.x(), right_t.x(), left_b.z(), right_t.z(), left_b.y(), m_p));
        sides.add_hitable(make_shared<XZRect>(left_b.x(), right_t.x(), left_b.z(), right_t.z(), right_t.y(), m_p));
    }

    virtual bool bounding_box(float t0, float t1, AABB &box) const override
    {
        box = AABB(left_b, right_t);
        return true;
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override
    {
        return sides.hit(r, t_min, t_max, rec);
    }
    /*data*/
    Vector3f left_b;       //左下角点
    Vector3f right_t;      //右上角点
    HitableList sides; //一个长方体的六个面
};
#endif
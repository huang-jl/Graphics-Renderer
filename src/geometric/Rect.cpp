#include "Rect.hpp"
/****************************************
 * XYRect
 ****************************************/
XYRect::XYRect(float x_0, float x_1, float y_0, float y_1, float z_, shared_ptr<Material> m_p) : Hitable(m_p), z(z_)
{
    x0 = imin(x_0, x_1);
    x1 = imax(x_0, x_1);
    y0 = imin(y_0, y_1);
    y1 = imax(y_0, y_1);
}

 bool XYRect::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    const float inv_dir = 1.0 / r.direction().z();
    float t = (z - r.origin().z()) * inv_dir; //根据平面的z坐标求出t
    if (t < t_min || t > t_max)
        return false;
    float x = r.origin().x() + r.direction().x() * t;
    float y = r.origin().y() + r.direction().y() * t;
    if (x < x0 || x > x1 || y < y0 || y > y1) //如果交点的x或者y坐标超出范围，则不相交
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    rec.p = r.point_at_parameter(t);
    rec.normal = Vector3f(0, 0, inv_dir > 0 ? -1 : 1);
    rec.material_p = material_p;
    return true;
}

 bool XYRect::bounding_box(float t0, float t1, AABB &box) const
{
    const float epsilon = 0.001;
    box = AABB(Vector3f(x0, y0, z - epsilon), Vector3f(x1, y1, z + epsilon));
    return true;
}

/****************************************
 * YZRect
 ****************************************/
YZRect::YZRect(float y_0, float y_1, float z_0, float z_1, float x_, shared_ptr<Material> m_p) : Hitable(m_p), x(x_)
{
    z0 = imin(z_0, z_1);
    z1 = imax(z_0, z_1);
    y0 = imin(y_0, y_1);
    y1 = imax(y_0, y_1);
}

 bool YZRect::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    const float inv_dir = 1.0 / r.direction().x();
    float t = (x - r.origin().x()) * inv_dir; //根据平面的x坐标求出t
    if (t < t_min || t > t_max)
        return false;
    float y = r.origin().y() + r.direction().y() * t;
    float z = r.origin().z() + r.direction().z() * t;
    if (y < y0 || y > y1 || z < z0 || z > z1) //如果交点的y或者z坐标超出范围，则不相交
        return false;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.p = r.point_at_parameter(t);
    rec.normal = Vector3f(inv_dir > 0 ? -1 : 1, 0, 0);
    rec.material_p = material_p;
    return true;
}

 bool YZRect::bounding_box(float t0, float t1, AABB &box) const
{
    const float epsilon = 0.001;
    box = AABB(Vector3f(x - epsilon, y0, z0), Vector3f(x + epsilon, y1, z1));
    return true;
}

/****************************************
 * XZRect
 ****************************************/
XZRect::XZRect(float x_0, float x_1, float z_0, float z_1, float y_, shared_ptr<Material> m_p) : Hitable(m_p), y(y_)
{
    x0 = imin(x_0, x_1);
    x1 = imax(x_0, x_1);
    z0 = imin(z_0, z_1);
    z1 = imax(z_0, z_1);
}
 bool XZRect::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    const float inv_dir = 1.0 / r.direction().y();
    float t = (y - r.origin().y()) * inv_dir; //根据平面的z坐标求出t
    if (t < t_min || t > t_max)
        return false;
    float x = r.origin().x() + r.direction().x() * t;
    float z = r.origin().z() + r.direction().z() * t;
    if (x < x0 || x > x1 || z < z0 || z > z1) //如果交点的x或者z坐标超出范围，则不相交
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.p = r.point_at_parameter(t);
    rec.normal = Vector3f(0, inv_dir > 0 ? -1 : 1, 0);
    rec.material_p = material_p;
    return true;
}
 bool XZRect::bounding_box(float t0, float t1, AABB &box) const
{
    const float epsilon = 0.001;
    box = AABB(Vector3f(x0, y - epsilon, z0), Vector3f(x1, y + epsilon, z1));
    return true;
}

/****************************************
 * Box
 ****************************************/
Box::Box(Vector3f left_bottom, Vector3f right_top, shared_ptr<Material> m_p)
    : Hitable(m_p), left_b(left_bottom), right_t(right_top)
{
    sides.add_hitable(make_shared<XYRect>(left_b.x(), right_t.x(), left_b.y(), right_t.y(), left_b.z(), m_p));
    sides.add_hitable(make_shared<XYRect>(left_b.x(), right_t.x(), left_b.y(), right_t.y(), right_t.z(), m_p));
    sides.add_hitable(make_shared<YZRect>(left_b.y(), right_t.y(), left_b.z(), right_t.z(), left_b.x(), m_p));
    sides.add_hitable(make_shared<YZRect>(left_b.y(), right_t.y(), left_b.z(), right_t.z(), right_t.x(), m_p));
    sides.add_hitable(make_shared<XZRect>(left_b.x(), right_t.x(), left_b.z(), right_t.z(), left_b.y(), m_p));
    sides.add_hitable(make_shared<XZRect>(left_b.x(), right_t.x(), left_b.z(), right_t.z(), right_t.y(), m_p));
}

 bool Box::bounding_box(float t0, float t1, AABB &box) const
{
    box = AABB(left_b, right_t);
    return true;
}

 bool Box::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    return sides.hit(r, t_min, t_max, rec);
}
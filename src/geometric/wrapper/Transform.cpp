#include "Transform.hpp"

/****************************************
 * Translate
 ****************************************/
Translate::Translate(shared_ptr<Hitable> o, const Vector3f &displacement) : object(o), offset(displacement) {}

bool Translate::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    Ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (object->hit(moved_r, t_min, t_max, rec))
    {
        rec.p += offset;
        return true;
    }
    else
    {
        return false;
    }
}

bool Translate::bounding_box(float t0, float t1, AABB &box) const
{
    AABB object_box;
    if (object->bounding_box(t0, t1, object_box))
    {
        box = AABB(object_box.min() + offset, object_box.max() + offset);
        return true;
    }
    else
    {
        return false;
    }
}

/****************************************
 * Rotate
 ****************************************/
Rotate::Rotate(shared_ptr<Hitable> o, float rotate_degree, int rotate_axis)
    : object(o), theta(degree_to_radius(rotate_degree)), axis(rotate_axis)
{
    cos_theta = cos(theta);
    sin_theta = sin(theta);
    Vector3f max(FLT_MIN, FLT_MIN, FLT_MIN);
    Vector3f min(FLT_MAX, FLT_MAX, FLT_MAX);
    if ((has_box = object->bounding_box(0, 1, b_box)))
    {
        //首先计算旋转后的AABB
        //思路是计算出9个顶点旋转后的情况
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                {
                    Vector3f temp;
                    temp.x() = i * b_box.min().x() + (1 - i) * b_box.max().x();
                    temp.y() = j * b_box.min().y() + (1 - j) * b_box.max().y();
                    temp.z() = k * b_box.min().z() + (1 - k) * b_box.max().z();
                    temp = axial_rotate(temp);
                    min = Vector3f(ffmin(temp.x(), min.x()), ffmin(temp.y(), min.y()), ffmin(temp.z(), min.z()));
                    max = Vector3f(ffmax(temp.x(), max.x()), ffmax(temp.y(), max.y()), ffmax(temp.z(), max.z()));
                }
    }
    b_box = AABB(min, max);
}

Vector3f Rotate::axial_rotate(const Vector3f &v) const
{
    auto r = [this](float &a, float &b) {
        float temp_a, temp_b;
        temp_a = cos_theta * a - sin_theta * b;
        temp_b = sin_theta * a + cos_theta * b;
        a = temp_a;
        b = temp_b;
    };
    Vector3f temp_v(v);
    if (axis == 0) //绕着x轴
        r(temp_v.y(), temp_v.z());
    else if (axis == 1) //绕着y轴
        r(temp_v.z(), temp_v.x());
    else //绕着z轴
        r(temp_v.x(), temp_v.y());
    return temp_v;
}
//绕着轴反向旋转
Vector3f Rotate::axial_rotate_reverse(const Vector3f &v) const
{
    auto r = [this](float &a, float &b) {
        float temp_a, temp_b;
        temp_a = cos_theta * a + sin_theta * b;
        temp_b = -sin_theta * a + cos_theta * b;
        a = temp_a;
        b = temp_b;
    };
    Vector3f temp_v(v);
    if (axis == 0) //绕着x轴
        r(temp_v.y(), temp_v.z());
    else if (axis == 1) //绕着y轴
        r(temp_v.z(), temp_v.x());
    else //绕着z轴
        r(temp_v.x(), temp_v.y());
    return temp_v;
}

bool Rotate::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    //首先把光线绕轴旋转
    Ray rotate_r(axial_rotate(r.origin()), axial_rotate(r.direction()), r.time());
    if (object->hit(rotate_r, t_min, t_max, rec))
    {
        rec.p = axial_rotate_reverse(rec.p);
        rec.normal = axial_rotate_reverse(rec.normal);
        return true;
    }
    else
        return false;
}

bool Rotate::bounding_box(float t0, float t1, AABB &box) const
{
    box = b_box;
    return has_box;
}

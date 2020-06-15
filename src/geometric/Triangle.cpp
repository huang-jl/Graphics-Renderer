#include "Triangle.hpp"

/****************************************
 * 采用重心坐标表示三角形P=u*V1+v*V2+(1-u-v)*V3
 * 对于直线P=O+t*D，连立求[D V3-V1 V3-V2](t u v)^T=V3-O
 * 令E1=V3-V1 E2=V3-V2 Q=V3-O
 * 则最后t=|V3-O V3-V1 V3-V2|/|D E1 E2|
 *      u=|D V3-O V3-V2|/|D E1 E2|
 *      v=|D V3-V1 V3-O|/|D E1 E2|
 * 注意行列式=混合积
 ****************************************/
Triangle::Triangle(Vector3f v1, Vector3f v2, Vector3f v3, shared_ptr<Material> m_p)
    : Hitable(m_p), E1(v3 - v1), E2(v3 - v2), V3(v3)
{
    E1_E2 = Vector3f::cross(E1, E2);
    normal = E1_E2.normalized();
}

bool Triangle::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
#ifdef DEBUG
    std::cout << V3 - E1 << " " << V3 - E2 << " " << V3 << "\t";
#endif
    Vector3f Q = V3 - r.origin();
    Vector3f D = r.direction();
    float det1 = Vector3f::dot(D, E1_E2); //|D E1 E2|
    if (fabs(det1) < EPS)
    {
        // std::cout << "Parallel\n";
        return false;
    }
    float det_t = Vector3f::dot(Q, E1_E2); //|V3-O V3-V1 V3-V2|
    Vector3f temp = Vector3f::cross(D, Q);
    float det_u = Vector3f::dot(E2, temp);
    float det_v = Vector3f::dot(E1, -temp);
    float t = det_t / det1;
    float u = det_u / det1;
    float v = det_v / det1;
    if (t < t_min || t > t_max || u < EPS || u > 1 || v < EPS || v > 1 || u + v > 1)
    {
#ifdef DEBUG
        std::cout << "Out of tri\n";
#endif
        return false;
    }

    // rec.normal = (Vector3f::dot(D, normal) > 0) ? -normal : normal;
    rec.t = t;
    rec.material_p = material_p;
    rec.p = r.point_at_parameter(t);
    rec.set_normal(r, normal);
#ifdef DEBUG
    std::cout << "origin = " << r.origin() << " direction = " << D << ", t = " << rec.t << ", normal = " << normal
              << ", point = " << rec.p << "\n";
#endif
    return true;
}

bool Triangle::bounding_box(float t0, float t1, AABB &box) const
{
    Vector3f V1 = V3 - E1;
    Vector3f V2 = V3 - E2;
    float x_min, y_min, z_min;
    float x_max, y_max, z_max;
    x_min = ffmin(V1.x(), ffmin(V2.x(), V3.x()));
    y_min = ffmin(V1.y(), ffmin(V2.y(), V3.y()));
    z_min = ffmin(V1.z(), ffmin(V2.z(), V3.z()));
    x_max = ffmax(V1.x(), ffmax(V2.x(), V3.x()));
    y_max = ffmax(V1.y(), ffmax(V2.y(), V3.y()));
    z_max = ffmax(V1.z(), ffmax(V2.z(), V3.z()));
    if (x_min == x_max)
    {
        x_min -= EPS;
        x_max += EPS;
    }
    if (y_min == y_max)
    {
        y_min -= EPS;
        y_max += EPS;
    }
    if (z_min == z_max)
    {
        z_min -= EPS;
        z_max += EPS;
    }
    box = AABB(Vector3f(x_min, y_min, z_min), Vector3f(x_max, y_max, z_max));
    return true;
}
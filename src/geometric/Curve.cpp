#include "Curve.hpp"
#include <cfloat>
#include <iostream>

BezierSurface::BezierSurface(const std::vector<Vector2f> &points, shared_ptr<Material> m_p)
    : Hitable(m_p), B_x(0), B_y(0)
{
    size_t n = points.size() - 1; // Bezier曲线的阶数
    for (int i = 0; i <= n; ++i)
    {
        auto B = combination(n, i) * Polynomial::bionomial(0, 1, i) * Polynomial::bionomial(1, -1, n - i);
        // std::cerr << points[i].x()<<" * (1-t)^" << n - i << " * t^" << i << "\n" << B*points[i].x() << "\n\n";
        B_x += B * points[i].x();
        B_y += B * points[i].y();
    }
    float min_x = FLT_MAX, min_y = FLT_MAX;
    float max_x = -FLT_MAX, max_y = -FLT_MAX;
    for (const auto &point : points)
    {
        min_x = ffmin(min_x, point.x());
        max_x = ffmax(max_x, point.x());
        min_y = ffmin(min_y, point.y());
        max_y = ffmax(max_y, point.y());
    }
    //默认Bezier曲线会绕着平行于y轴的直线旋转
    b_box = AABB(Vector3f(-max_x, min_y, -max_x), Vector3f(max_x, max_y, max_x));
    std::cerr << b_box.min() << "\n" << b_box.max() << "\n";
    std::cerr << "B(0.33) = (" << B_x(0.33) << ", " << B_y(0.33) << ") B(0.75) = " << B_x(0.75) << ", " << B_y(0.75)
              << ")\n";
    std::cerr << "B'(0) = (" << B_x.derivative(0) << ", " << B_y.derivative(0) << ") B'(1) = " << B_x.derivative(1)
              << ", " << B_y.derivative(1) << ")\n";
}

bool BezierSurface::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    //坐标为(B_x(u)cos o, B_y(u), B_x(u)sin o)
    //光线为(origin.x + t*dir.x, origin.y + t*dir.y, origin.z + t*dir.z)
    //使用牛顿迭代法求解，初始值为光线和层次包围盒交点
    float t, u = get_rand(), theta;
    if (!b_box.hit(r, t_min, t_max, &t))
        return false;
    theta = atan2(r.origin().z() + r.direction().z() * t, r.origin().x() + r.direction().x() * t);
    if (!NewtonIter(r.origin(), r.direction(), t, u, theta))
        return false;
    if (t < t_min || t > t_max || u < 0 || u > 1)
        return false;
    rec.t = t;
    Vector2f R(B_y.derivative(u), -B_x.derivative(u));
    rec.normal = Vector3f(cos(theta) * R.x(), R.y(), sin(theta) * R.x());
    rec.normal.normalize();
    rec.p = r.point_at_parameter(t);
    // std::cerr << "Ray = " << rec.p << ", Bezier = (" << B_x(u) * cos(theta) << ", " << B_y(u) << ", "
    //           << B_x(u) * sin(theta) << ")\n";
    rec.material_p = material_p;
    return true;
}

bool BezierSurface::bounding_box(float t0, float t1, AABB &box) const
{
    box = b_box;
    return true;
}

bool BezierSurface::NewtonIter(const Vector3f &origin, const Vector3f &dir, float &t, float &u, float &theta, float tol,
                               const int MAX_ITER) const
{
    const float epsilon = 1e-2;
    for (int i = 0; i < MAX_ITER; ++i)
    {
        float cos_th = cos(theta);
        float sin_th = sin(theta);
        float Bx_u = B_x(u);
        float By_u = B_y(u);
        Vector3f F(Bx_u * cos_th - origin.x() - dir.x() * t, By_u - origin.y() - dir.y() * t,
                   Bx_u * sin_th - origin.z() - dir.z() * t);
        //构建雅可比矩阵
        Matrix3f J_F(-dir.x(), B_x.derivative(u) * cos_th, -sin_th * Bx_u, -dir.y(), B_y.derivative(u), 0, -dir.z(),
                     B_x.derivative(u) * sin_th, cos_th * Bx_u);
        bool is_singular;
        Matrix3f J_F_inv = J_F.inverse(&is_singular, 1e-2);
        if (is_singular)
        {
            // std::cerr << "Singular!\n";
            u = get_rand();
            continue;
        }
        Vector3f delta = J_F_inv * F;

        if (fabsf(delta.x()) < tol && fabsf(delta.y()) < tol && fabsf(delta.z()) < tol)
            return true;
        // if (i == 0)
        // {
        //     std::cerr << "t = " << t << ", u = " << u << ", theta = " << theta << "\n";
        //     std::cerr << "F = " << F << "\n";
        //     std::cerr << "Delta = " << delta << "\n";
        // }
        t -= delta.x();
        u -= delta.y();
        theta -= delta.z();
    }
    Vector3f p = origin + t * dir;
    return fabsf(p.x() - B_x(u) * cos(theta)) < epsilon && fabsf(p.y() - B_y(u)) < epsilon &&
           fabsf(p.z() - B_x(u) * sin(theta)) < epsilon;
}
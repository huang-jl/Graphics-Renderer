#ifndef CURVE_HPP
#define CURVE_HPP

#include "BezierFunc.hpp"
#include "Hitable.hpp"
#include <utility>
#include <vector>
/****************************************
 * 参数曲线曲面
 *      包括Bezier曲线曲面
 ****************************************/

/****************************************
 * Bezier曲面
 * 默认是绕y轴旋转，所给控制点为二维(x,y)的样式
 ****************************************/
class BezierSurface : public Hitable
{
  public:
    BezierSurface(const std::vector<Vector2f> &points, shared_ptr<Material> m_p);

    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    bool NewtonIter(const Vector3f &origin, const Vector3f &dir, float &t, float &u, float &theta, float tol = EPS,
                    const int MAX_ITER = 50) const;
    /*data*/
    //对应Bezier曲线的多项式
    Polynomial B_x;     //对应横坐标
    Polynomial B_y;     //对应纵坐标
    AABB b_box;
};

#endif
#ifndef ONB_HPP
#define ONB_HPP
#include "vecmath.h"

/****************************************
 * Ortho-normal Bases
 * 用于重要性采样，生成随机方向
 * 建立一个关于交点法向的标准正交基
 ****************************************/
class ONB
{
  public:
    ONB() {}
    inline Vector3f u() const { return axis[0]; }
    inline Vector3f v() const { return axis[1]; }
    inline Vector3f w() const { return axis[2]; }
    inline Vector3f local(const float a, const float b, const float c) const
    {
        return a * axis[0] + b * axis[1] + c * axis[2];
    }
    inline Vector3f local(const Vector3f &cord) const
    {
        return cord[0] * axis[0] + cord[1] * axis[1] + cord[2] * axis[2];
    }
    void build_from_w(const Vector3f &n) //利用n生成
    {
        Vector3f a; //利用n和与n不平行的向量构建一个ONB
        axis[2] = n.normalized();
        if (fabs(axis[2].x()) > 0.9)    //fabs是必须的
            a = Vector3f(0, 1, 0);
        else
            a = Vector3f(1, 0, 0);

        axis[1] = Vector3f::cross(axis[2], a).normalized();
        axis[0] = Vector3f::cross(axis[2], axis[1]);
    }

    /*data*/
    Vector3f axis[3];
};
#endif
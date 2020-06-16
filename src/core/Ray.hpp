#ifndef RAY_HPP
#define RAY_HPP
#include <vecmath.h>

/*
 * 为了支持运动模糊效果
 * 我们需要对每根光线指定一个时间
 * 用于确定光线追踪时，该根光线
 */
class Ray
{
  public:
    Ray() {}
    Ray(const Vector3f &a, const Vector3f &b, float ti = 0.0) : o(a), dir(b), time_(ti) {}
    Vector3f origin() const { return o; }
    Vector3f direction() const { return dir; }
    Vector3f point_at_parameter(float t) const { return o + t * dir; }
    void change_dir(const Vector3f &v) { dir = v; }
    void change_origin(const Vector3f &origin_) { o = origin_; }

    float time() const { return time_; }

    Vector3f o;
    Vector3f dir;
    float time_;
};
#endif
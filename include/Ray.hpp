#ifndef RAY_HPP
#define RAY_HPP
#include "vec3.hpp"

/*
 * 为了支持运动模糊效果
 * 我们需要对每根光线指定一个时间
 * 用于确定光线追踪时，该根光线
 */
class Ray
{
  public:
    Ray() {}
    Ray(const vec3 &a, const vec3 &b, float ti = 0.0) : o(a), dir(b), time_(ti) {}
    vec3 origin() const { return o; }
    vec3 direction() const { return dir; }
    vec3 point_at_parameter(float t) const { return o + t * dir; }
    float time() const { return time_; }

    vec3 o;
    vec3 dir;
    float time_;
};
#endif
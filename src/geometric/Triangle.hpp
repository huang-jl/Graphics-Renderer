#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP
#include "Hitable.hpp"

/****************************************
 * 采用重心坐标表示三角形P=u*V1+v*V2+(1-u-v)*V3
 * 对于直线P=O+t*D，连立求[D V3-V1 V3-V2](t u v)^T=V3-O
 * 令E1=V3-V1 E2=V3-V2 Q=V3-O
 * 则最后t=|V3-O V3-V1 V3-V2|/|D E1 E2|
 *      u=|D V3-O V3-V2|/|D E1 E2|
 *      v=|D V3-V1 V3-O|/|D E1 E2|
 * 注意行列式=混合积
 * 
 * 这里默认v1,v2,v3顺时针
****************************************/
class Triangle : public Hitable
{
  public:
    Triangle(Vector3f v1, Vector3f v2, Vector3f v3, shared_ptr<Material> m_p);
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    /*data*/
    Vector3f normal;    //三角面片法向量
    Vector3f E1, E2, V3;
    Vector3f E1_E2;     //cross(E1, E2)
};
#endif
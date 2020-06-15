#ifndef MESH_HPP
#define MESH_HPP
#include "BVH.hpp"
#include "Hitable.hpp"
#include <vector>

class Mesh : public Hitable
{
  public:
    Mesh(const char *filename, float scale, shared_ptr<Material> m_p);
    //求交函数，返回值表示是否相交
    virtual bool hit(const Ray &r, float t_min, float t_max, Hit &rec) const override;
    //计算包围盒的函数，返回值表示该物体是否有包围盒
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    /*data*/
    std::vector<Vector3f> vertex;
    shared_ptr<BVHNode> mesh;
};
#endif
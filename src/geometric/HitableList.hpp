#ifndef HITABLE_LIST_HPP
#define HITABLE_LIST_HPP
#include "Hitable.hpp"
#include <cstdio>
#include <vector>

class HitableList : public Hitable
{
  public:
    HitableList() {}
    HitableList(std::vector<shared_ptr<Hitable>> &l) : list(l) {}
    virtual bool hit(const Ray &r, float tmin, float tmax, Hit &rec) const override;

    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
    inline void add_hitable(shared_ptr<Hitable> object) { list.push_back(object); }
    /*data*/
    std::vector<shared_ptr<Hitable>> list;
};

#endif
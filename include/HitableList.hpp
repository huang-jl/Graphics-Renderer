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
    virtual bool hit(const Ray &r, float tmin, float tmax, Hit &rec) const override
    {
        Hit temp_rec;
        bool hit_anything = false;
        double closed_so_far = tmax;
        for (auto &object_p : list)
        {
            if (object_p->hit(r, tmin, closed_so_far, temp_rec))
            {
                hit_anything = true;
                closed_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }

    inline virtual bool bounding_box(float t0, float t1, AABB &box) const override
    {
        if (list.empty())
            return false;
        //下面考察list中的每个物体的包围盒，再返回包围盒的包围盒
        //有一个物体没有包围盒，则list没有包围盒
        AABB temp_box;
        bool first_true = true;
        for (const auto object : list)
        {
            if (object->bounding_box(t0, t1, temp_box))
            {
                box = first_true ? temp_box : surrounding_box(box, temp_box);
                first_true = false;
            }
            else
                return false;
        }
        return true;
    }
    void add_hitable(shared_ptr<Hitable> object) { list.push_back(object); }
    /*data*/
    std::vector<shared_ptr<Hitable>> list;
};

#endif
#include "BVH.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>

bool box_compare_(const shared_ptr<Hitable> a, const shared_ptr<Hitable> b, int axis)
{
    AABB box_a, box_b;
    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BVHNode constructor!\n";
    return box_a.min()[axis] < box_b.min()[axis];
}

bool box_x_compare_(const shared_ptr<Hitable> a, const shared_ptr<Hitable> b) { return box_compare_(a, b, 0); }
bool box_y_compare_(const shared_ptr<Hitable> a, const shared_ptr<Hitable> b) { return box_compare_(a, b, 1); }
bool box_z_compare_(const shared_ptr<Hitable> a, const shared_ptr<Hitable> b) { return box_compare_(a, b, 2); }

BVHNode::BVHNode(std::vector<shared_ptr<Hitable>> &l, size_t start, size_t end, float time0, float time1)
{
    if ((end - start) == 1)
        left_c = right_c = l[start];
    else if ((end - start) == 2)
    {
        left_c = l[start];
        right_c = l[start + 1];
    }
    else
    {
        int axis = static_cast<int>(3 * get_rand());
        auto comparator = (axis == 0) ? box_x_compare_ : (axis == 1) ? box_y_compare_ : box_z_compare_;
        // if (axis == 0)
        //     std::sort(l.begin() + start, l.begin() + end, box_x_compare_);
        // else if (axis == 1)
        //     std::sort(l.begin() + start, l.begin() + end, box_y_compare_);
        // else
        //     std::sort(l.begin() + start, l.end() + end, box_z_compare_);
        std::sort(l.begin() + start, l.begin() + end, comparator);
        size_t mid = (start + end) / 2;
        left_c = std::make_shared<BVHNode>(l, start, mid, time0, time1);
        right_c = std::make_shared<BVHNode>(l, mid, end, time0, time1);
    }
    AABB box_left, box_right;
    if (!left_c->bounding_box(time0, time1, box_left) || !right_c->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in BVHNode constructor\n";
    box = surrounding_box(box_left, box_right);
}

//返回每个节点的包围盒
bool BVHNode::bounding_box(float t0, float t1, AABB &output_box) const
{
    output_box = box;
    return true;
}

//递归求交点
bool BVHNode::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    if (!box.hit(r, t_min, t_max))
    {
#ifdef DEBUG
// std::cout << "Box not hit : origin = " << r.origin() << " dir = " << r.direction() << "\t box = ";
// std::cout << box.min() << " " << box.max() << "\n";
#endif
        return false;
    }
    bool left_result = left_c->hit(r, t_min, t_max, rec);
    bool right_result = right_c->hit(r, t_min, left_result ? rec.t : t_max, rec);
    return left_result || right_result;
}

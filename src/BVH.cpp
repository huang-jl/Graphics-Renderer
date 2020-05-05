#include "BVH.hpp"
#include <algorithm>
#include <cstdlib>

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
    int axis = static_cast<int>(3 * get_rand());
    if (axis == 0)
        // qsort(l, n, sizeof(Hitable *), box_x_compare);
        std::sort(l.begin(), l.end(), box_x_compare_);
    else if (axis == 1)
        // qsort(l, n, sizeof(Hitable *), box_y_compare);
        std::sort(l.begin(), l.end(), box_y_compare_);
    else
        // qsort(l, n, sizeof(Hitable *), box_z_compare);
        std::sort(l.begin(), l.end(), box_z_compare_);
    if ((end - start) == 1)
        left_c = right_c = l[start];
    else if ((end - start) == 2)
    {
        left_c = l[start];
        right_c = l[start+1];
    }
    else
    {
        left_c = std::make_shared<BVHNode>(l, start, start + (end - start) / 2, time0, time1);
        right_c = std::make_shared<BVHNode>(l, start + (end - start) / 2, end, time0, time1);
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
        return false;
    bool left_result = left_c->hit(r, t_min, t_max, rec);
    bool right_result = right_c->hit(r, t_min, left_result ? rec.t : t_max, rec);
    return left_result || right_result;
}

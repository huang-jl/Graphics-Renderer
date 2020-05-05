#include "BVH.hpp"
#include <cstdlib>

int box_x_compare(const void *a, const void *b)
{
    AABB box_a, box_b;
    Hitable *hitable_a = *(Hitable **)a;
    Hitable *hitable_b = *(Hitable **)b;
    if (!hitable_a->bounding_box(0, 0, box_a) || !hitable_a->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BHVNode constructor!\n";
    if (box_a.min().x() < box_b.min().y())
        return -1;
    else
        return 1;
}

int box_y_compare(const void *a, const void *b)
{
    AABB box_a, box_b;
    Hitable *hitable_a = *(Hitable **)a;
    Hitable *hitable_b = *(Hitable **)b;
    if (!hitable_a->bounding_box(0, 0, box_a) || !hitable_a->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BHVNode constructor!\n";
    if (box_a.min().y() < box_b.min().y())
        return -1;
    else
        return 1;
}

int box_z_compare(const void *a, const void *b)
{
    AABB box_a, box_b;
    Hitable *hitable_a = *(Hitable **)a;
    Hitable *hitable_b = *(Hitable **)b;
    if (!hitable_a->bounding_box(0, 0, box_a) || !hitable_a->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BHVNode constructor!\n";
    if (box_a.min().z() < box_b.min().z())
        return -1;
    else
        return 1;
}
//这里排序的目的是为了让包围盒各自尽可能重叠的比较少
//否则随机的选取个体做包围盒可能父亲和子代包围盒都很大，导致效果很差
BHVNode::BHVNode(Hitable **l, int n, float time0, float time1)
{
    int axis = static_cast<int>(3 * get_rand());
    if (axis == 0)
        qsort(l, n, sizeof(Hitable *), box_x_compare);
    else if (axis == 1)
        qsort(l, n, sizeof(Hitable *), box_y_compare);
    else
        qsort(l, n, sizeof(Hitable *), box_z_compare);
    if (n == 1)
        left_c = right_c = l[0];
    else if (n == 2)
    {
        left_c = l[0];
        right_c = l[1];
    }
    else
    {
        left_c = new BHVNode(l, n / 2, time0, time1);
        right_c = new BHVNode(l + n / 2, n - n / 2, time0, time1);
    }
    AABB box_left, box_right;
    if (!left_c->bounding_box(time0, time1, box_left) || !right_c->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in BHVNode constructor\n";
    box = surrounding_box(box_left, box_right);
}

//返回每个节点的包围盒
bool BHVNode::bounding_box(float t0, float t1, AABB &output_box) const
{
    output_box = box;
    return true;
}

//递归求交点
bool BHVNode::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    if (!box.hit(r, t_min, t_max))
        return false;
    bool left_result = left_c->hit(r, t_min, t_max, rec);
    bool right_result = right_c->hit(r, t_min, left_result ? rec.t : t_max, rec);
    return left_result || right_result;
}

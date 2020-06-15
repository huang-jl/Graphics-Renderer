#include "Media.hpp"

ConstantMedium::ConstantMedium(shared_ptr<Hitable> o, float d, shared_ptr<Texture> t) : boundary(o), density(d)
{
    material_p = make_shared<Isotropic>(t);
}

bool ConstantMedium::hit(const Ray &r, float t_min, float t_max, Hit &rec) const
{
    /* 假设光线在\delta L路径发生散射的概率为C\delta L，C为散射密度
     * 因此光线散射距离大于L的概率为P(x>L)=(1-C\delta L)^(L/\delta L)
     * 当\delta L趋于0后可以知道P(x>L)=e^{-CL}
     * 通过均匀分布u产生这个分布的方法为用-(log u)/C 模拟x
     * P(-(log u)/C > L)=P(u<e^{-CL})=e^{-CL}
     *
     * 1.先求出和两个物体的两个交点p1,p2
     * 2.判断交点是否在(t_min,t_max)之间
     * 3.随机生成物体的散射距离
     * 4.判断散射距离和|p1-p2|的关系看是否会散射
     */
    Hit rec1, rec2;
    const float epsilon = 0.001;
    if (!boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
        return false;
    if (!boundary->hit(r, rec1.t + epsilon, FLT_MAX, rec2))
        return false;
    rec1.t = ffmax(rec1.t, t_min); //随机散射区域下界为max{交点1，t_min,0}
    rec1.t = ffmax(0, rec1.t);
    rec2.t = ffmin(rec2.t, t_max); //随机散射区域上界为min{交点2，t_max}
    if (rec1.t >= rec2.t)
        return false;

    float distance_in_boundary = (rec2.t - rec1.t) * r.direction().length();
    float scatter_distance = -log(get_frand()) / density;
    if (scatter_distance < distance_in_boundary)
    {
        rec.t = rec1.t + scatter_distance / r.direction().length();
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = Vector3f(1, 0, 0); //不重要的参数，与散射光线无关
        rec.material_p = material_p;
        return true;
    }
    else
        return false;
}

bool ConstantMedium::bounding_box(float t0, float t1, AABB &box) const { return boundary->bounding_box(t0, t1, box); }
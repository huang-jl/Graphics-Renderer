#include "Sphere.hpp"

/****************************************
 * Sphere
 ****************************************/

/****************************************
 * 对球面进行UV纹理贴图
 * input:
 * 交点的坐标p，待求的u，v坐标
 *
 ****************************************/
void Sphere::get_sphere_uv(const Vector3f &p, float &u, float &v)
{
    float phi = atan2(p.z(), p.x()); //经度，绕着两级旋转的角度
    float theta = asin(p.y());       //纬度，即法线与赤道平面夹角
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

bool Sphere::hit(const Ray &r, float tmin, float tmax, Hit &rec) const
{
    //参略一些可以约去的2
    Vector3f oc = r.origin() - center; //球心到光线原点的向量
    float a = Vector3f::dot(r.direction(), r.direction());
    float b = Vector3f::dot(r.direction(), oc); // assert(b<0)
    float c = Vector3f::dot(oc, oc) - radius * radius;
    float delta = b * b - a * c;

    if (delta > 0)
    {
        float temp = (-b - sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            Sphere::get_sphere_uv(rec.p - center, rec.u, rec.v);
            return true;
        }
        temp = (-b + sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            Sphere::get_sphere_uv(rec.p - center, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

/****************************************
 * Moving Sphere
 ****************************************/
bool MovingSphere::hit(const Ray &r, float tmin, float tmax, Hit &rec) const
{
    //获得光线所在时刻球体的位置
    Vector3f center = get_center(r.time());

    Vector3f oc = r.origin() - center; //球心到光线原点的向量
    float a = Vector3f::dot(r.direction(), r.direction());
    float b = Vector3f::dot(r.direction(), oc); // assert(b<0)
    float c = Vector3f::dot(oc, oc) - radius * radius;
    float delta = b * b - a * c;

    if (delta > 0)
    {
        float temp = (-b - sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            return true;
        }
        temp = (-b + sqrt(delta)) / a;
        if (temp > tmin && temp < tmax)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius; //单位化
            rec.material_p = material_p;
            return true;
        }
    }
    return false;
}

/*
 * 计算包围盒的函数
 * t0,t1表示两个时间
 * 向box中填入包围盒的左下角和右上角点
 * 返回是否有包围盒
 */
bool Sphere::bounding_box(float t0, float t1, AABB &box) const
{
    box = AABB(center - Vector3f(radius, radius, radius), center + Vector3f(radius, radius, radius));
    return true;
}

/*
 * 计算包围盒的函数
 * t0,t1表示两个时间
 * 向box中填入包围盒的左下角和右上角点
 * 返回是否有包围盒
 *
 * 同时因为是运动的球体，我们同时也需要计算出
 * 能够包围住两个时间点形成的球体的大包围盒
 */
bool MovingSphere::bounding_box(float t0, float t1, AABB &box) const
{
    //已知两个包围盒，求大包围盒的lambda表达式

    AABB box0 = AABB(center0 - Vector3f(radius, radius, radius), center0 + Vector3f(radius, radius, radius));
    AABB box1 = AABB(center1 - Vector3f(radius, radius, radius), center1 + Vector3f(radius, radius, radius));
    box = surrounding_box(box0, box1);
    return true;
}
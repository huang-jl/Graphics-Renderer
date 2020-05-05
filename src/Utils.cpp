#include "Utils.hpp"
#include "BVH.hpp"
#include "Hitable.hpp"
#include "HitableList.hpp"
#include "Material.hpp"
#include "Sphere.hpp"
#include <vector>

float ffmin(float a, float b) { return (a < b) ? a : b; }
float ffmax(float a, float b) { return (a < b) ? b : a; }
/******************************
 光线追踪的关键函数
    input:
        r——待求交的光线
        world——物体，是一个Hitable list
        depth——递归深度
 ******************************/
vec3 color(const Ray &r, shared_ptr<Hitable> world, int depth)
{
    Hit rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec))
    {
        //根据法向量构建颜色，最后的图像颜色和法向量有关(x,y,z)<=>(r,g,b)
        //采用递归的方式
        //有散射的物体本身没有颜色，它的颜色通过其之后的光线射中的位置确定
        //这里的attenuation是反射的系数，它会吸收相应的光线能量
        Ray reflect_r;
        vec3 attenuation;
        if (depth < MAX_DEPTH && rec.material_p->scatter(r, rec, attenuation, reflect_r))
            return attenuation * color(reflect_r, world, depth + 1);
        else
            return vec3(0, 0, 0);
        // return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
    }
    // else
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

shared_ptr<Hitable> generate_scene()
{
    // int n = 500;
    // int num = 0;
    std::vector<shared_ptr<Hitable>> list;
    list.push_back(make_shared<Sphere>(vec3(0, -1000, 0), 1000, make_shared<Lambertian>(vec3(0.5, 0.5, 0.5))));
    for (int i = -11; i < 11; ++i)
        for (int j = -11; j < 11; ++j)
        {
            float choosed = get_rand();
            vec3 center(i + 0.9 * get_rand(), 0.2, j + 0.9 * get_rand());
            if ((center - vec3(4, 0.2, 0)).length() < 0.9)
                continue;
            if (choosed < 0.8)
                list.push_back(make_shared<MovingSphere>(
                    center, center + vec3(0, 0.5 * get_rand(), 0.0), 0, 1, 0.2,
                                     make_shared<Lambertian>(vec3(get_rand() * get_rand(), get_rand() * get_rand(),
                                                                     get_rand() * get_rand()))));
            else if (choosed < 0.95)
                list.push_back(make_shared<Sphere>(
                    center, 0.2,
                    make_shared<Metal>(vec3(0.5 * (get_rand() + 1), 0.5 * (get_rand() + 1), 0.5 * (get_rand() + 1)),
                                          0.5 * get_rand())));
            else
                list.push_back(make_shared<Sphere>(center, 0.2, make_shared<Dielectric>(1.5)));
        }
    list.push_back(make_shared<Sphere>(vec3(0, 1, 0), 1.0, make_shared<Dielectric>(1.5)));
    list.push_back(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, make_shared<Lambertian>(vec3(0.4, 0.2, 0.1))));
    list.push_back(make_shared<Sphere>(vec3(4, 1, 0), 1.0, make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0)));
    return make_shared<BVHNode>(list, 0, list.size(), 0, 1);
    // return new HitableList(list, num);
}

/******************************
 反射的关键函数
    input:
        v——入射光
        normal——反射光
    return:
        反射光的方向
 ******************************/
vec3 reflect(const vec3 &v, const vec3 &normal)
{
    // v表示入射光线，normal表示交点法向量
    // assert(dot(v, normal) < 0);
    return v - 2 * dot(v, normal) * normal;
}

/******************************
 折射的关键函数
    input:
        v——入射光
        normal——法向量
        n——relative——入射光所在介质折射率/另一侧介质折射率
    output:
        refracted——折射光线
    return:
        是否能够折射
 ******************************/
bool refract(const vec3 &v, const vec3 &normal, float n_relative, vec3 &refracted)
{
    vec3 unit_v = unit_vector(v);
    //入射角为beta
    float cos_beta = dot(unit_v, normal);
    //折射角alpha，则discriminant = cos^2(alpha)
    float discriminant = 1.0 - n_relative * n_relative * (1 - cos_beta * cos_beta);
    if (discriminant > 0.0)
    {
        //能够发生折射，根据折射的关系，利用正弦定理可以推导如下
        // refracted = n_relative * unit_v - (sqrt(discriminant) + n_relative * cos_beta) * normal;
        refracted = n_relative * (unit_v - normal * cos_beta) - normal * sqrt(discriminant);
        return true;
    }
    else
        return false;
}

/******************************
 反射比计算函数
 用来计算折射时有多少光线会反射
 为了模拟真实感
    input:
        cosine——入射角的余弦
        ref_idx——折射率
    return:
        反射比，表示反射的大小（可以理解成概率）
 ******************************/
float schlick(float cosine, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow(1.0 - cosine, 5);
}

float get_rand()
{
    static std::default_random_engine engine;
    static std::uniform_real_distribution<float> dis(0.0, 1.0);
    return dis(engine);
}

vec3 random_in_unit_sphere()
{
    vec3 point(0, 0, 0);
    do
    {
        point = 2 * vec3(get_rand(), get_rand(), get_rand()) - vec3(1, 1, 1);
    } while (point.squared_length() > 1);
    return point;
}

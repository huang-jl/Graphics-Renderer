#include "Utils.hpp"
#include "BVH.hpp"
#include "Curve.hpp"
#include "Hitable.hpp"
#include "HitableList.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Media.hpp"
#include "Rect.hpp"
#include "Sphere.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include <ctime>
#include <vector>

float ffmin(float a, float b) { return (a < b) ? a : b; }
float ffmax(float a, float b) { return (a < b) ? b : a; }
int imin(int a, int b) { return (a < b) ? a : b; }
int imax(int a, int b) { return (a > b) ? a : b; }
/******************************
 光线追踪的关键函数
    input:
        r——待求交的光线
        world——物体，是一个Hitable
        depth——递归深度
 ******************************/
Vector3f color(const Ray &r, shared_ptr<Hitable> world, int depth)
{
    Hit rec;
    //当交点处的t<0.001的时候，认为不相交，从而能绘出阴影的效果
    if (world->hit(r, 0.001, FLT_MAX, rec))
    {
        //根据法向量构建颜色，最后的图像颜色和法向量有关(x,y,z)<=>(r,g,b)
        //采用递归的方式
        //有散射的物体本身没有颜色，它的颜色通过其之后的光线射中的位置确定
        //这里的attenuation是反射的系数，它会吸收相应的光线能量
        Ray reflect_r;
        Vector3f attenuation;
        Vector3f emitted = rec.material_p->emitted(rec.u, rec.v, rec.p);
        if (depth < MAX_DEPTH && rec.material_p->scatter(r, rec, attenuation, reflect_r))
            return emitted + attenuation * color(reflect_r, world, depth + 1);
        else
            return emitted;
        // return 0.5 * Vector3f(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
    }
    // else
    // Vector3f unit_direction = (r.direction());
    // float t = 0.5 * (unit_direction.y() + 1.0);
    // return (1.0 - t) * Vector3f(1.0, 1.0, 1.0) + t * Vector3f(0.5, 0.7, 1.0);
    return Vector3f(0, 0, 0); //纯黑背景
}

shared_ptr<Hitable> generate_scene()
{
    std::vector<shared_ptr<Hitable>> list;
    auto checker_texture = make_shared<CheckerTexture>(make_shared<ConstantTexture>(Vector3f(0.2, 0.3, 0.1)),
                                                       make_shared<ConstantTexture>(Vector3f(0.9, 0.9, 0.9)));
    list.push_back(make_shared<Sphere>(Vector3f(0, -1000, 0), 1000, make_shared<Lambertian>(checker_texture)));
    for (int i = -11; i < 11; ++i)
        for (int j = -11; j < 11; ++j)
        {
            float choosed = get_rand();
            Vector3f center(i + 0.9 * get_rand(), 0.2, j + 0.9 * get_rand());
            if ((center - Vector3f(4, 0.2, 0)).length() < 0.9)
                continue;
            if (choosed < 0.8)
                list.push_back(make_shared<MovingSphere>(
                    center, center + Vector3f(0, 0.5 * get_rand(), 0.0), 0, 1, 0.2,
                    make_shared<Lambertian>(
                        Vector3f(get_rand() * get_rand(), get_rand() * get_rand(), get_rand() * get_rand()))));
            else if (choosed < 0.95)
                list.push_back(make_shared<Sphere>(
                    center, 0.2,
                    make_shared<Metal>(Vector3f(0.5 * (get_rand() + 1), 0.5 * (get_rand() + 1), 0.5 * (get_rand() + 1)),
                                       0.5 * get_rand())));
            else
                list.push_back(make_shared<Sphere>(center, 0.2, make_shared<Dielectric>(1.5)));
        }
    list.push_back(make_shared<Sphere>(Vector3f(0, 1, 0), 1.0, make_shared<Dielectric>(1.5)));
    list.push_back(make_shared<Sphere>(Vector3f(-4, 1, 0), 1.0, make_shared<Lambertian>(Vector3f(0.4, 0.2, 0.1))));
    list.push_back(make_shared<Sphere>(Vector3f(4, 1, 0), 1.0, make_shared<Metal>(Vector3f(0.7, 0.6, 0.5), 0.0)));
    return make_shared<BVHNode>(list, 0, list.size(), 0, 1);
    // return new HitableList(list, num);
}

shared_ptr<Hitable> two_sphere()
{
    // auto checker_texture = make_shared<CheckerTexture>(make_shared<ConstantTexture>(Vector3f(0.2,0.3,0.1)),
    // make_shared<ConstantTexture>(Vector3f(0.9,0.9,0.9)));
    shared_ptr<Texture> noise_texture = make_shared<NoiseTexture>(3);
    std::vector<shared_ptr<Hitable>> list;
    shared_ptr<Hitable> s1 = make_shared<Sphere>(Vector3f(0, -1000, 0), 1000, make_shared<Lambertian>(noise_texture));
    shared_ptr<Hitable> s2 = make_shared<Sphere>(Vector3f(0, 2, 0), 2, make_shared<Lambertian>(noise_texture));
    list.push_back(s1);
    list.push_back(s2);
    return make_shared<HitableList>(list);
}

shared_ptr<Hitable> simple_light() //测试光源
{
    std::vector<shared_ptr<Hitable>> list;
    shared_ptr<Texture> noise_texture = make_shared<NoiseTexture>(3);
    auto s1 = make_shared<Sphere>(Vector3f(0, -1000, 0), 1000, make_shared<Lambertian>(noise_texture));
    auto s2 = make_shared<Sphere>(Vector3f(0, 2, 0), 2, make_shared<Lambertian>(noise_texture));
    auto s3 = make_shared<Sphere>(Vector3f(0, 7, 0), 2,
                                  make_shared<DiffuseLight>(make_shared<ConstantTexture>(Vector3f(4, 4, 4))));
    auto s4 =
        make_shared<XYRect>(3, 5, 1, 3, -2, make_shared<DiffuseLight>(make_shared<ConstantTexture>(Vector3f(4, 4, 4))));
    list.push_back(s1);
    list.push_back(s2);
    list.push_back(s3);
    list.push_back(s4);
    return make_shared<HitableList>(list);
}

shared_ptr<Hitable> cornell_box() // cornell box测试场景
{
    std::vector<shared_ptr<Hitable>> list;
    shared_ptr<Material> red = make_shared<Lambertian>(Vector3f(0.65, 0.05, 0.05));
    shared_ptr<Material> white = make_shared<Lambertian>(Vector3f(0.73, 0.73, 0.73));
    shared_ptr<Material> green = make_shared<Lambertian>(Vector3f(0.12, 0.45, 0.15));
    shared_ptr<Material> light = make_shared<DiffuseLight>(Vector3f(7, 7, 7));

    shared_ptr<Hitable> box_1 = make_shared<Translate>(
        make_shared<Rotate>(make_shared<Box>(Vector3f(0, 0, 0), Vector3f(135, 135, 135), white), 40, 1),
        Vector3f(130, 0, 65));
    // shared_ptr<Hitable> box_2 = make_shared<Translate>(
    //     make_shared<Rotate>(make_shared<Box>(Vector3f(0, 0, 0), Vector3f(165, 330, 165), white), -35, 1),
    //     Vector3f(265, 0, 295));

    // shared_ptr<Hitable> box_1 = make_shared<Translate>(
    //     make_shared<Rotate>(
    //         make_shared<ConstantMedium>(make_shared<Box>(Vector3f(0, 0, 0), Vector3f(165, 165, 165), white), 0.01,
    //                                     make_shared<ConstantTexture>(Vector3f(1, 1, 1))),
    //         -18, 1),
    //     Vector3f(130, 0, 65));
    // shared_ptr<Hitable> box_2 = make_shared<Translate>(
    //     make_shared<Rotate>(
    //         make_shared<ConstantMedium>(make_shared<Box>(Vector3f(0, 0, 0), Vector3f(165, 330, 165), white), 0.01,
    //                                     make_shared<ConstantTexture>(Vector3f(0, 0, 0))),
    //         15, 1),
    //     Vector3f(265, 0, 295));

    // shared_ptr<Hitable> fog_1 = make_shared<ConstantMedium>(box_1, 0.01, make_shared<ConstantTexture>(1, 1, 1));
    // shared_ptr<Hitable> fog_2 = make_shared<ConstantMedium>(box_2, 0.01, make_shared<ConstantTexture>(0, 0, 0));
    list.push_back(make_shared<YZRect>(0, 555, 0, 555, 555, green));
    list.push_back(make_shared<YZRect>(0, 555, 0, 555, 0, red));
    list.push_back(make_shared<XZRect>(113, 443, 127, 432, 554, light));
    list.push_back(make_shared<XZRect>(0, 555, 0, 555, 0, white));
    list.push_back(make_shared<XYRect>(0, 555, 0, 555, 555, white));
    list.push_back(make_shared<XZRect>(0, 555, 0, 555, 555, white));
    list.push_back(box_1);
    // list.push_back(box_2);
    std::vector<Vector2f> v{Vector2f(0, 0), Vector2f(47, -5.7), Vector2f(96, 52.8), Vector2f(0, 140)};
    shared_ptr<BezierSurface> bezier_p = make_shared<BezierSurface>(v, make_shared<Lambertian>(Vector3f(1,1,1)));
    shared_ptr<Translate>t_b = make_shared<Translate>(bezier_p, Vector3f(400,10,200));
    list.push_back(t_b);
    std::cerr << "Init over\n";
    return make_shared<HitableList>(list);
}

/******************************
 反射的关键函数
    input:
        v——入射光
        normal——反射光
    return:
        反射光的方向
 ******************************/
Vector3f reflect(const Vector3f &v, const Vector3f &normal)
{
    // v表示入射光线，normal表示交点法向量
    // assert(Vector3f::dot(v, normal) < 0);
    return v - 2 * Vector3f::dot(v, normal) * normal;
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
bool refract(const Vector3f &v, const Vector3f &normal, float n_relative, Vector3f &refracted)
{
    Vector3f unit_v = v.normalized();
    //入射角为beta
    float cos_beta = Vector3f::dot(unit_v, normal);
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
    static std::default_random_engine engine(time(NULL));
    static std::uniform_real_distribution<float> dis(0.0, 1.0);
    return dis(engine);
}

Vector3f random_in_unit_sphere()
{
    Vector3f point(0, 0, 0);
    do
    {
        point = 2 * Vector3f(get_rand(), get_rand(), get_rand()) - Vector3f(1, 1, 1);
    } while (point.squaredLength() > 1);
    return point;
}

float degree_to_radius(float degree) //角度制转弧度制
{
    return degree * M_PI / 180;
}
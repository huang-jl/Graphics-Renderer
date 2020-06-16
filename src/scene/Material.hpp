#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Hitable.hpp"
#include "ONB.hpp"
#include "PDF.hpp"
#include "Ray.hpp"
#include "Texture.hpp"
#include "Utils.hpp"

/****************************************
 * 记录散射光线的信息
 * 支持重要性采样
 ****************************************/
struct ScatterRecord
{
    /* data */
    Ray specular_ray;
    bool is_specular;        //是否是镜面光线
    Vector3f attenuation;    //衰减率
    shared_ptr<PDF> pdf_ptr; //重要性采样类
};

/*
 * 为了支持运动模糊，注意散射光线的时刻和入射光线一致
 */
class Material
{
  public:
    virtual ~Material(){};
    // input:输入光线、交点信息
    // output:(r,g,b)的衰减量，散射光线
    virtual bool scatter(const Ray &r_in, const Hit &rec, ScatterRecord &scattered) const { return false; };
    virtual float scattering_pdf(const Ray &r_in, const Hit &rec, const Ray &scattered) const
    {
        return 0;
    }; //散射光线的概率密度
    //光源材质需要重写的虚函数，默认材质是不带任何颜色的
    virtual Vector3f emitted(const Hit &rec, float u, float v, Vector3f &p) const { return Vector3f(0, 0, 0); }
    virtual float get_bright()const { return 0; } //返回光源的亮度
};

class Lambertian : public Material
{
    /* 该材质即漫反射的材质
     * 会随机产生一条散射光线：
     * 在和交点处相切，半径为1的球体中随机取一个点target
     * 连接交点和target为随机产生的光线
     */
  public:
    Lambertian(shared_ptr<Texture> a) : albedo(a) {}
    Lambertian(Vector3f a) { albedo = make_shared<ConstantTexture>(a); }

    virtual float scattering_pdf(const Ray &r_in, const Hit &rec, const Ray &scattered) const //散射光线的密度函数
    {
        //密度函数为散射光和法向量夹角的余弦cos/PI
        float cosine = Vector3f::dot(rec.normal, scattered.direction().normalized());
        if (cosine < 0) //散射光线应该和法向量成锐角
            cosine = 0;
        return cosine / M_PI;
    }

    virtual bool scatter(const Ray &r_in, const Hit &rec, ScatterRecord &scattered) const
    {
        scattered.is_specular = false;
        scattered.attenuation = albedo->value(rec.u, rec.v, rec.p);
        scattered.pdf_ptr = make_shared<CosinePDF>(rec.normal); //以法向量为z轴构建坐标系
        return true;
    }

    /*data*/
    // Vector3f albedo; //反射率
    shared_ptr<Texture> albedo;
};

/****************************************
 * 金属材质，需要
 *      Vector——表示光线吸收量
 *      f——表示模糊效果，反射会在分布在
 *          以f为半径的球中
 ****************************************/
class Metal : public Material
{
  public:
    // a表示吸收率，f表示模糊效果的大小
    Metal(const Vector3f &a, float f = 0) : albedo(a) { fuzz = (f < 1) ? f : 1; }
    virtual bool scatter(const Ray &r_in, const Hit &rec, ScatterRecord &scattered) const override
    {
        Vector3f reflected = reflect(r_in.direction().normalized(), rec.normal); //根据反射定律获得反射光
        scattered.is_specular = true;
        scattered.attenuation = albedo;
        //反射光线会加上一个fuzz的扰动，不让表面过于尖锐
        scattered.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        scattered.pdf_ptr = nullptr;
        return true;
    }
    /*data*/
    Vector3f albedo;
    float fuzz; //模糊效果
};

/****************************************
 * 电解质材质，需要一个参数表示折射率
 ****************************************/
class Dielectric : public Material
{
    //电解质材质，不会吸收任何能量，因此attenuation = 1
  public:
    Dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const Ray &r_in, const Hit &rec, ScatterRecord &scattered) const override
    {
        scattered.attenuation = Vector3f(1.0, 1.0, 1.0); //不吸收任何能量
        scattered.is_specular = true;
        scattered.pdf_ptr = nullptr;
        float n_relative = rec.front_face ? (1.0 / ref_idx) : ref_idx;

        Vector3f unit_dir = r_in.direction().normalized();
        float cosine = ffmin(Vector3f::dot(-unit_dir, rec.normal), 1.0);
        float sine = sqrt(1 - cosine * cosine);
        if (n_relative * sine > 1)
        {
            Vector3f reflected = reflect(unit_dir, rec.normal);
            scattered.specular_ray = Ray(rec.p, reflected, r_in.time());
            return true;
        }
        float reflect_prob = schlick(cosine, ref_idx);
        if (get_frand() < reflect_prob)
        {
            Vector3f reflected = reflect(unit_dir, rec.normal);
            scattered.specular_ray = Ray(rec.p, reflected, r_in.time());
            return true;
        }
        else
        {
            Vector3f refracted = refract(unit_dir, rec.normal, n_relative, cosine);
            scattered.specular_ray = Ray(rec.p, refracted, r_in.time());
        }
        return true;
    }

    /*index*/
    float ref_idx;
};

/****************************************
 * 各向同性材质，产生一条完全随机的光线
 *      输入参数为计算反射比的材质
 * 和Lambertian不同，漫反射至少是和入射光线
 * 在同一侧
 ****************************************/
class Isotropic : public Material
{
  public:
    Isotropic(shared_ptr<Texture> a) : albedo(a) {}
    virtual bool scatter(const Ray &r_in, const Hit &rec, Vector3f &attenuation, Ray &scattered) const
    {
        scattered = Ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
    /*data*/
    shared_ptr<Texture> albedo; //反射比
};

#endif
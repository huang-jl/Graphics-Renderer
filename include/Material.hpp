#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Hitable.hpp"
#include "Ray.hpp"
#include "Utils.hpp"

/*
 * 为了支持运动模糊，注意散射光线的时刻和入射光线一致
 */
class Material
{
  public:
    // input:输入光线、交点信息
    // output:(r,g,b)的衰减量，散射光线
    virtual bool scatter(const Ray &r_in, const Hit &rec, vec3 &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material
{
    /* 该材质即漫反射的材质
     * 会随机产生一条散射光线：
     * 在和交点处相切，半径为1的球体中随机取一个点target
     * 连接交点和target为随机产生的光线
     */
  public:
    Lambertian(const vec3 &a) : albedo(a) {}
    virtual bool scatter(const Ray &r_in, const Hit &rec, vec3 &attenuation, Ray &scattered) const
    {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = Ray(rec.p, target - rec.p, r_in.time());
        attenuation = albedo;
        return true;
    }

    /*data*/
    vec3 albedo; //反射率
};

class Metal : public Material
{
  public:
    // a表示吸收率，f表示模糊效果的大小
    Metal(const vec3 &a, float f = 0) : albedo(a) { fuzz = (f < 1) ? f : 1; }
    virtual bool scatter(const Ray &r_in, const Hit &rec, vec3 &attenuation, Ray &scattered) const
    {
        //反射光线会随机的加上一个散布在球体中的向量
        scattered = Ray(rec.p, reflect(r_in.direction(), rec.normal) + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        //从外界射入才会反射
        return dot(scattered.direction(), rec.normal) > 0;
    }
    /*data*/
    vec3 albedo;
    float fuzz; //模糊效果
};

class Dielectric : public Material
{
    //电解质材质，不会吸收任何能量，因此attenuation = 1
  public:
    Dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const Ray &r_in, const Hit &rec, vec3 &attenuation, Ray &scattered) const
    {
        attenuation = vec3(1.0, 1.0, 1.0); //不吸收任何能量
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float n_relative;
        vec3 refracted;
        float cosine, reflect_prob;
        if (dot(r_in.direction(), rec.normal) > 0)
        {
            //从物体的内部折射
            outward_normal = -rec.normal;
            n_relative = ref_idx;
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else
        {
            //外界射到物体上
            outward_normal = rec.normal;
            n_relative = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        //计算反射比
        if (refract(r_in.direction(), outward_normal, n_relative, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else
        {
            reflect_prob = 1.0;
        }
        //根据反射比求之后的散射光线
        if (get_rand() < reflect_prob)
        {
            scattered = Ray(rec.p, reflected, r_in.time());
        }
        else
        {
            scattered = Ray(rec.p, refracted, r_in.time());
        }
        return true;
    }

    /*index*/
    float ref_idx;
};
#endif
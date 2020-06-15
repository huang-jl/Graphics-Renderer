#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Hitable.hpp"
#include "Material.hpp"

/*
 * 这里的光源为了方便
 * 把它作为材质Material的子类，能够方便地放置到各种Hitbale上
 * 从而可以之间创建出各种形状的光源
 *
 * 可以认为光源 = Hitbale + Light_Material
 * 和光源有强相关的Hitable和Material都在这个头文件里
 */

/****************************************
 * 基础的根据材质颜色发光的光源
 ****************************************/
class DiffuseLight : public Material
{
  public:
    DiffuseLight() {}
    DiffuseLight(Vector3f a, Vector3f d = Vector3f::ZERO) : emit(make_shared<ConstantTexture>(a)){};
    DiffuseLight(shared_ptr<Texture> t_p, Vector3f d = Vector3f::ZERO) : emit(t_p) {}
    //默认没有散射，因为是光源
    virtual bool scatter(const Ray &r_in, const Hit &rec, ScatterRecord&) const override
    {
        return false;
    }

    //根据材质的颜色发光
    virtual Vector3f emitted(const Hit &rec, float u, float v, Vector3f &p) const override
    {
        if(rec.front_face)
            return emit->value(u, v, p);
        else
            return Vector3f::ZERO;
    }

    /*data*/
    shared_ptr<Texture> emit; //用材质的颜色作为光源
};

#endif
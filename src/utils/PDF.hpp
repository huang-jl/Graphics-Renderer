#ifndef PDF_HPP
#define PDF_HPP

#include "Hitable.hpp"
#include "ONB.hpp"
#include "Utils.hpp"
#include "vecmath.h"

/****************************************
 * 概率密度函数的抽象类
 * 主要完成两个功能
 *  1.判断一个方向的pdf
 *  2.随机生成一个方向
 ****************************************/
class PDF
{
  public:
    virtual float value(const Vector3f &) const = 0;
    virtual Vector3f generate() const = 0;
    virtual ~PDF() {}
};

/****************************************
 * p(direction) = cos(theta)/M_PI
 * 随机产生一条半球面中的光线（利用极坐标）
 ****************************************/
class CosinePDF : public PDF
{
  public:
    CosinePDF(const Vector3f &w) { uvw.build_from_w(w); }
    virtual float value(const Vector3f &ref_dir) const override
    {
        float cosine = Vector3f::dot(ref_dir.normalized(), uvw.w());
        return cosine < 0 ? 0 : cosine / M_PI;
    }

    virtual Vector3f generate() const override { return uvw.local(random_cosine_direction()); }
    /*data*/
    ONB uvw;
};

/****************************************
 * 用于产生指向物体（例如光源）的光线
 ****************************************/
class HitablePDF : public PDF
{
  public:
    HitablePDF(shared_ptr<Hitable> h_p, const Vector3f &o) : origin(o), target(h_p) {}
    virtual float value(const Vector3f &dir) const override { return target->pdf_value(origin, dir); }

    virtual Vector3f generate() const override { return target->random(origin); }

    /*data*/
    Vector3f origin;
    shared_ptr<Hitable> target;
};

/****************************************
 * 两种概率密度的平均
 ****************************************/
class MixturePDF : public PDF
{
  public:
    MixturePDF(shared_ptr<PDF> p1, shared_ptr<PDF> p2)
    {
        pdf[0] = p1;
        pdf[1] = p2;
    }

    virtual float value(const Vector3f &dir) const override
    {
        return 0.5 * pdf[0]->value(dir) + 0.5 * pdf[1]->value(dir);
    }

    virtual Vector3f generate() const override
    {
        if (get_frand() < 0.5)
        {
            return pdf[0]->generate();
        }
        else
        {
            return pdf[1]->generate();
        }
    }

    /*data*/
    shared_ptr<PDF> pdf[2];
};
#endif
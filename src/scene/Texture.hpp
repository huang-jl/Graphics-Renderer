#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "Perlin.hpp"
#include "Utils.hpp"
#include <vecmath.h>

class Texture
{
  public:
    virtual ~Texture() {}
    virtual Vector3f value(float u, float v, const Vector3f &p) const = 0;
};

/* 这个纹理是一个颜色固定的纹理
 * 用在材质上可以形成固定反射率的材质，例如漫反射材质
 */
class ConstantTexture : public Texture
{
  public:
    ConstantTexture() {}
    ConstantTexture(float a, float b, float c) : color(a, b, c) {}
    ConstantTexture(Vector3f c) : color(c) {}
    virtual Vector3f value(float u, float v, const Vector3f &p) const override { return color; }

    /*data*/
    Vector3f color;
};

/* 棋盘格纹理
 * 基本思想是用两个纹理轮流交替
 */
class CheckerTexture : public Texture
{
  public:
    CheckerTexture() {}
    CheckerTexture(shared_ptr<Texture> t1, shared_ptr<Texture> t2) : odd(t1), even(t2) {}
    virtual Vector3f value(float u, float v, const Vector3f &p) const override
    {
        float sine = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sine < EPS)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

    /*data*/
    shared_ptr<Texture> odd;
    shared_ptr<Texture> even;
};

class NoiseTexture : public Texture
{
  public:
    NoiseTexture(int scale_ = 1) : scale(scale_) {}
    virtual Vector3f value(float u, float v, const Vector3f &p) const override
    {
        //采用向量点乘平滑后的柏林噪声输出为(-1,1)，需要映射为(0,1)
        // return 0.5 * (1.0 + noise.noise(scale * p)) * Vector3f(1, 1, 1);

        //采用扰动噪声则默认的返回范围接近(0,2)
        // return noise.turb_noise(scale*p) * Vector3f(1, 1, 1);

        /*
         * 间接采用扰动噪声，将其作用在sin的相(phase)上，起到波纹效果,类似大理石
         * 这里加了一个初始相位p.x()，会使波纹方向朝向比较一致
         * 可以修改初始相位，使大理石纹理朝着自己期望的方向伸展
         */
        return 0.5 * (1 + sin(scale * p.x() + 10 * noise.turb_noise(p))) * Vector3f(1, 1, 1);
    }
    /*data*/
    Perlin noise;
    int scale;
};

class ImageTexture : public Texture
{
  public:
    ImageTexture(unsigned char *pixels, int width, int height) : data(pixels), nx(width), ny(height) {}
    virtual Vector3f value(float u, float v, const Vector3f &p) const override
    {
        int i = static_cast<int>(u * nx);
        int j = static_cast<int>((1 - v) * ny);
        i = (i < EPS) ? 0 : i;
        j = (j < EPS) ? 0 : j;
        i = (i > nx - 1) ? nx - 1 : i;
        j = (j > ny - 1) ? ny - 1 : j;
        float r = static_cast<int>(data[3 * i + 3 * nx * j]) / 255.0;
        float g = static_cast<int>(data[3 * i + 3 * nx * j + 1]) / 255.0;
        float b = static_cast<int>(data[3 * i + 3 * nx * j + 2]) / 255.0;
        return Vector3f(r, g, b);
    }
    /*data*/
    unsigned char *data;
    int nx, ny; //图片的宽度和高度
};

#endif
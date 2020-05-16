#ifndef PERLIN_NOISE
#define PERLIN_NOISE

#include "Utils.hpp"
#include <cmath>
/*
 * 柏林噪声
 * 采用hash的方式，对一个点Vector3f&p返回一个float
 * 需要明确，柏林噪声应该让相邻位置的颜色尽可能比较接近
 * 为了使返回值更平滑，我们采用线性差值的方法
 *
 * 为了不让柏林噪声的变化过低
 * 可以考虑放大传入的坐标p为scale*p
 * 为了减缓马赫带(Mach bands)的影响，即格子的痕迹比较明显
 * 采用hermite cube的方法
 * 主要是线性插值的时候将u,v,w做一个变换u*u*(3-2*u)
 */

/*
 * 具体的插值方法是：
 * 用p的周围八个整点格计算对应的柏林噪声值
 * 最后根据p的（小数）大小插值
 *
 * 为了避免最大值和最小值出现在整数点上，导致出现类似网格的样式
 * 采用了一个权重向量来点乘上随机向量，使纹理更加平滑·
 */
float linear_interpolation(Vector3f value[2][2][2], float u, float v, float w);

class Perlin
{
  public:
    Perlin();
    ~Perlin();

    float noise(const Vector3f &p) const;

    /*
     * 扰动噪声：由多个频率不同的噪声混合而成
     * mix_degree代表混合的程度
     * 返回值范围接近(0,2)
     *
     * 通常是间接使用
     */
    float turb_noise(const Vector3f &p, int mix_degree = 7) const;

  private:
    /*data*/
    static const int POINT_NUM = 256;
    Vector3f *ranfloat;
    int *perm_x;
    int *perm_y;
    int *perm_z;

    Vector3f *perlin_generate();

    void permute(int *array);

    int *perlin_generate_perm();
};
#endif
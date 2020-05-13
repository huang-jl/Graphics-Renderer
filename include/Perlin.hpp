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
inline float linear_interpolation(Vector3f value[2][2][2], float u, float v, float w)
{
    float accum = 0.0;
    auto hermite_cube = [](float x) { return x * x * (3 - 2 * x); };
    float uu = hermite_cube(u);
    float vv = hermite_cube(v);
    float ww = hermite_cube(w);
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
            {
                Vector3f weight_vector = Vector3f(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) *
                         Vector3f::dot(value[i][j][k], weight_vector);
            }
    return accum;
}

class Perlin
{
  public:
    Perlin()
    {
        ranfloat = perlin_generate();
        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
        permute(perm_x);
        permute(perm_y);
        permute(perm_z);
    }
    ~Perlin()
    {
        delete[] ranfloat;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    float noise(const Vector3f &p) const
    {
        float u = p.x() - floor(p.x());
        float v = p.y() - floor(p.y());
        float w = p.z() - floor(p.z());
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());

        Vector3f value[2][2][2];
        for (int di = 0; di < 2; ++di)
            for (int dj = 0; dj < 2; ++dj)
                for (int dk = 0; dk < 2; ++dk)
                    value[di][dj][dk] =
                        ranfloat[perm_x[(i + di) & (POINT_NUM - 1)] ^ perm_y[(j + dj) & (POINT_NUM - 1)] ^
                                 perm_z[(k + dk) & (POINT_NUM - 1)]];
        return linear_interpolation(value, u, v, w);
    }

    /*
     * 扰动噪声：由多个频率不同的噪声混合而成
     * mix_degree代表混合的程度
     * 返回值范围接近(0,2)
     * 
     * 通常是间接使用
     */
    float turb_noise(const Vector3f &p, int mix_degree = 7) const
    {
        float accum = 0.0;
        Vector3f temp = p;
        float weight = 1.0;
        for (int i = 0; i < mix_degree; ++i)
        {
            accum += weight * noise(temp);  //范围是一个有限项等比数列求和
            temp *= 2;
            weight *= 0.5;
        }
        return fabs(accum); //返回值接近fabs(-2,2)
    }

  private:
    /*data*/
    static const int POINT_NUM = 256;
    Vector3f *ranfloat;
    int *perm_x;
    int *perm_y;
    int *perm_z;

    Vector3f *perlin_generate()
    {
        Vector3f *rand_f = new Vector3f[POINT_NUM];
        for (int i = 0; i < POINT_NUM; ++i)
            rand_f[i] = (2 * Vector3f(get_rand(), get_rand(), get_rand()) - Vector3f(1, 1, 1)).normalized();
        return rand_f;
    }

    void permute(int *array)
    {
        for (int i = POINT_NUM - 1; i > 0; --i)
        {
            int target = static_cast<int>(get_rand() * POINT_NUM);
            std::swap(array[target], array[i]);
        }
    }

    int *perlin_generate_perm()
    {
        int *rand_i = new int[POINT_NUM];
        for (int i = 0; i < POINT_NUM; ++i)
            rand_i[i] = i;
        return rand_i;
    }
};
#endif
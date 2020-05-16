#include "Perlin.hpp"

float linear_interpolation(Vector3f value[2][2][2], float u, float v, float w)
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

Perlin::Perlin()
{
    ranfloat = perlin_generate();
    perm_x = perlin_generate_perm();
    perm_y = perlin_generate_perm();
    perm_z = perlin_generate_perm();
    permute(perm_x);
    permute(perm_y);
    permute(perm_z);
}
Perlin::~Perlin()
{
    delete[] ranfloat;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
}

float Perlin::noise(const Vector3f &p) const
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
                value[di][dj][dk] = ranfloat[perm_x[(i + di) & (POINT_NUM - 1)] ^ perm_y[(j + dj) & (POINT_NUM - 1)] ^
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
float Perlin::turb_noise(const Vector3f &p, int mix_degree) const
{
    float accum = 0.0;
    Vector3f temp = p;
    float weight = 1.0;
    for (int i = 0; i < mix_degree; ++i)
    {
        accum += weight * noise(temp); //范围是一个有限项等比数列求和
        temp *= 2;
        weight *= 0.5;
    }
    return fabs(accum); //返回值接近fabs(-2,2)
}

Vector3f *Perlin::perlin_generate()
{
    Vector3f *rand_f = new Vector3f[POINT_NUM];
    for (int i = 0; i < POINT_NUM; ++i)
        rand_f[i] = (2 * Vector3f(get_rand(), get_rand(), get_rand()) - Vector3f(1, 1, 1)).normalized();
    return rand_f;
}

void Perlin::permute(int *array)
{
    for (int i = POINT_NUM - 1; i > 0; --i)
    {
        int target = static_cast<int>(get_rand() * POINT_NUM);
        std::swap(array[target], array[i]);
    }
}

int *Perlin::perlin_generate_perm()
{
    int *rand_i = new int[POINT_NUM];
    for (int i = 0; i < POINT_NUM; ++i)
        rand_i[i] = i;
    return rand_i;
}
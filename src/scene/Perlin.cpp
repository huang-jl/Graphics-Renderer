#include "Perlin.hpp"
#include "Utils.hpp"

double perlin_interp(Vector3f value[2][2][2], double u, double v, double w)
{
    double uu = u * u * (3 - 2 * u);
    double vv = v * v * (3 - 2 * v);
    double ww = w * w * (3 - 2 * w);
    double acc = 0.0;

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
            {
                Vector3f weight_vec(u - i, v - j, w - k);
                acc += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) *
                       Vector3f::dot(value[i][j][k], weight_vec);
            }
    return acc;
}

Perlin::Perlin()
{
    ranvec = new Vector3f[POINT_NUM];
    for (int i = 0; i < POINT_NUM; ++i)
        ranvec[i] = (2 * Vector3f(get_frand(), get_frand(), get_frand()) - Vector3f(1, 1, 1)).normalized();
    perm_x = perlin_generate_perm();
    perm_y = perlin_generate_perm();
    perm_z = perlin_generate_perm();
}

Perlin::~Perlin()
{
    delete[] ranvec;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
}

double Perlin::noise(const Vector3f &p) const
{
    auto u = p.x() - floor(p.x());
    auto v = p.y() - floor(p.y());
    auto w = p.z() - floor(p.z());
    int i = static_cast<int>(floor(p.x()));
    int j = static_cast<int>(floor(p.y()));
    int k = static_cast<int>(floor(p.z()));

    Vector3f value[2][2][2];
    for (int di = 0; di < 2; ++di)
        for (int dj = 0; dj < 2; ++dj)
            for (int dk = 0; dk < 2; ++dk)
            {
                value[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
            }
    return perlin_interp(value, u, v, w);
}

int *Perlin::perlin_generate_perm()
{
    int *array = new int[POINT_NUM];
    for (int i = 0; i < POINT_NUM; ++i)
        array[i] = i;
    permute(array, POINT_NUM);
    return array;
}

void Perlin::permute(int *array, int n)
{
    for (int i = POINT_NUM - 1; i > 0; --i)
    {
        int target = get_irand(0, i);
        int temp = array[i];
        array[i] = array[target];
        array[target] = temp;
    }
}

double Perlin::turb(const Vector3f &p, int mix_degree)const
{
    double accum = 0.0;
    Vector3f temp = p;
    double weight = 1.0;
    for (int i = 0; i < mix_degree; ++i)
    {
        accum += noise(temp) * weight;
        weight *= 0.5;
        temp *= 2.0;
    }
    return fabs(accum);
}
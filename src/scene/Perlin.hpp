#ifndef PERLIN_HPP
#define PERLIN_HPP

#include "vecmath.h"

double perlin_interp(Vector3f value[2][2][2], double u, double v, double w);
class Perlin
{
  public:
    Perlin();
    ~Perlin();
    double noise(const Vector3f &p) const;
    double turb(const Vector3f &p, int mix_degree = 7)const;

    static int *perlin_generate_perm();
    static void permute(int *p, int n);
    /*data*/
    static const int POINT_NUM = 256;
    Vector3f *ranvec;
    int *perm_x;
    int *perm_y;
    int *perm_z;
};
#endif
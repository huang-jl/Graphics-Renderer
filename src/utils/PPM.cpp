#include "PPM.hpp"

PPM::PPM(int width_, int height_, int max_depth) : width(width_), height(height_), MAX_DEPTH(max_depth)
{
    pic = new Vector3f *[height_];
    for (int i = 0; i < height_; ++i)
        pic[i] = new Vector3f[width_];
}

PPM::~PPM()
{
    for (int i = 0; i < height; ++i)
        delete[] pic[i];
    delete[] pic;
}

void PPM::ray_tracing(const Ray &r, const Vector3f &ray_color);
{
    for (int i = 0; i < MAX_DEPTH; ++i)
    {
        Hit rec;
    }
}
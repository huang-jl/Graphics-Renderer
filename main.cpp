#include "Camera.hpp"
#include "Hitable.hpp"
#include "HitableList.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Utils.hpp"
#include <cfloat>
#include <iostream>

int main()
{
    int ny = 800;
    float aspect = 1.0;
    int nx = ny * aspect;
    int ns = 50; //我们的采样个数
    Vector3f look_from = Vector3f(278, 278, -800);
    Vector3f look_at = Vector3f(278, 278, 0);
    float focus_d = 10;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    Camera camera(look_from, look_at, Vector3f(0, 1, 0), 40, aspect, 0.0, focus_d, 0, 1);
    shared_ptr<Hitable> world = cornell_box();
    for (int j = ny - 1; j >= 0; --j)
        for (int i = 0; i < nx; ++i)
        {
            Vector3f col(0, 0, 0);
            //抗锯齿采样
            for (int s = 0; s < ns; ++s)
            {
                //这个地方很重要，将(u,v)映射为(0,1)范围，这样nx和ny
                //就只是图像的一个大小比例，而不会影响图像的显示范围
                float u = float(i + get_rand()) / float(nx);
                float v = float(j + get_rand()) / float(ny);
                Ray r = camera.get_ray(u, v);
                col += color(r, world, 0);
            }
            col = col / float(ns);
            col = Vector3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //使用gamma2校验
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            // if (ir > 255 || ig > 255 || ib > 255)
            // std::cerr << "Value more than 255\n";
            std::cout << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
            // std::cout << ir << " " << ig << " " << ib << "\n";
        }
}
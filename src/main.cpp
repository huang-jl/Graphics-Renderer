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
    int ny = 300;
    float aspect = 16.0 / 9.0;
    int nx = ny * aspect;
    int ns = 200; //我们的采样个数
    vec3 look_from = vec3(278, 278, -800);
    vec3 look_at = vec3(278, 278, 0);
    float focus_d = 10;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    Camera camera(look_from, look_at, vec3(0, 1, 0), 40, aspect, 0.0, focus_d, 0, 1);
    // Hitable *list[5];
    // list[0] = new Sphere(vec3(0, 0, -1), 0.5, new Lambertian(vec3(0.1, 0.2, 0.5)));
    // list[1] = new Sphere(vec3(0, -100.5, -1), 100, new Lambertian(vec3(0.8, 0.8, 0.0)));
    // list[2] = new Sphere(vec3(1, 0, -1), 0.5, new Metal(vec3(0.8, 0.6, 0.2)));
    // list[3] = new Sphere(vec3(-1, 0, -1), 0.5, new Dielectric(1.5));
    // list[4] = new Sphere(vec3(-1, 0, -1), -0.45, new Dielectric(1.5));
    // Hitable *world = new HitableList(list, 5);
    shared_ptr<Hitable> world = cornell_box();
    // world = generate_scene();
    for (int j = ny - 1; j >= 0; --j)
        for (int i = 0; i < nx; ++i)
        {
            vec3 col(0, 0, 0);
            //抗锯齿采样
            for (int s = 0; s < ns; ++s)
            {
                float u = float(i + get_rand()) / float(nx);
                float v = float(j + get_rand()) / float(ny);
                Ray r = camera.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //使用gamma2
            const float max = ffmax(col[0], ffmax(col[1], col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            // if (ir > 255 || ig > 255 || ib > 255)
            // std::cerr << "Value more than 255\n";
            // std::cout << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
}
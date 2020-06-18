#include "Controller.hpp"
#include "PPM.hpp"
#include "Perlin.hpp"
#include "Utils.hpp"
#include <cfloat>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
    // int width = 500, height = 500;
    // int samples = 100;
    // const auto aspect_ratio = float(width) / height;
    // Vector3f lookfrom(13, 2, 3);
    // Vector3f lookat(0, 0, 0);
    // Vector3f vup(0, 1, 0);
    // float vfov = 20.0;
    // auto dist_to_focus = 10.0;
    // auto aperture = 0.0;

    // Camera camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    // shared_ptr<Hitable> world = two_sphere();

    // std::ofstream output("./perlin.ppm", std::ios::out | std::ios::trunc);
    // output << "P3\n" << width << " " << height << "\n255\n";
    // for (int j = height - 1; j >= 0; --j)
    // {
    //     std::cout << j << "\r" << std::flush;
    //     for (int i = 0; i < width; ++i)
    //     {
    //         Vector3f pixel_color;
    //         for (int s = 0; s < samples; ++s)
    //         {
    //             auto u = (i + get_frand()) / (width - 1);
    //             auto v = (j + get_frand()) / (height - 1);
    //             Ray r = camera.get_ray(u, v);
    //             pixel_color += color(r, world, nullptr, 0);
    //         }
    //         pixel_color *= float(1.0 / samples);
    //         pixel_color = Vector3f(sqrt(pixel_color[0]), sqrt(pixel_color[1]), sqrt(pixel_color[2]));
    //         int ir = int(255.99 * pixel_color[0]);
    //         int ig = int(255.99 * pixel_color[1]);
    //         int ib = int(255.99 * pixel_color[2]);
    //         output << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
    //     }
    // }

    if (argc != 2)
    {
        std::cout << "Usage: PT <path to scene.json>\n";
        exit(1);
    }
    PPM ppm(argv[1]);
    ppm.run();
    std::ofstream output(ppm.controller.save_path, std::ios::out | std::ios::trunc);
    output << "P3\n" << ppm.width << " " << ppm.height << "\n255\n";
    for (int y = ppm.height - 1; y >= 0; --y)
        for (int x = 0; x < ppm.width; ++x)
        {
            Vector3f col = ppm.pic[y][x];
            col = Vector3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); // gamma=2
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            // if (ir > 255 || ig > 255 || ib > 255)
            // std::cerr << "Value more than 255\n";
            output << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
            // std::cout << ir << " " << ig << " " << ib << "\n";
        }
    output.close();

    // Controller controller;
    // if (argc == 2)
    // {
    //     controller.parse(argv[1]);
    // }
    // else
    // {
    //     std::cout << "Usage: PT <path to scene.json>\n";
    //     exit(1);
    // }
    // controller.generate_pic();

    return 0;
}
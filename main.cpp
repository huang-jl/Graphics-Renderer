#include "PPM.hpp"
#include "Parser.hpp"
#include "Perlin.hpp"
#include "Render.hpp"
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

    Parser parser;
    if (argc != 2)
    {
        std::cout << "Usage: PT <path to scene.json>\n";
        exit(1);
    }
    SceneInfo info = parser.parse(argv[1]);
    Render render(info);
    render.render();
    return 0;
}
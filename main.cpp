#include "Controller.hpp"
#include "PPM.hpp"
#include "Utils.hpp"
#include <cfloat>
#include <iostream>

int main(int argc, char *argv[])
{
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
            const Vector3f &col = ppm.pic[y][x];
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
    // }
    // controller.generate_pic();
    return 0;
}
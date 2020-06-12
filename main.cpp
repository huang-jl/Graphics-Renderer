#include "Controller.hpp"
#include "Utils.hpp"
#include <cfloat>
#include <iostream>

int main(int argc, char *argv[])
{
    // int ny = 500;
    // float aspect = 1.0;
    // int nx = ny * aspect;
    // int ns = 200; //我们的采样个数
    // Vector3f look_from = Vector3f(278, 278, -800);
    // Vector3f look_at = Vector3f(278, 278, 0);
    // float focus_d = 10;

    Controller controller;
    // controller.set_imgae_param(ny, aspect, ns);
    // controller.set_camera_param(look_from, look_at, Vector3f(0, 1, 0), 40, 0.0, focus_d, 0, 1);
    // controller.set_scene(cornell_box());
    if(argc == 2)
    {
        controller.parse(argv[1]);
    }
    else
    {
        std::cout << "Usage: PT <path to scene.json>\n";
        exit(1);
    }
    controller.generate_pic();
    // Ray r = controller.camera.get_ray(0.7, 0.3);
    // Vector3f c = color(r, controller.world, 0);
    // std::cout << c <<"\n";
    return 0;
}
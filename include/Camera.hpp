#ifndef CAMARA_HPP
#define CAMARA_HPP
#include "Ray.hpp"
#include "Utils.hpp"
class Camera
{
    /* 这里的的参数都是像素/100后的值
     * 例如horizontal表示图片的水平方向向量，实际为4.0*100=400 pixel
     * 因为相机的位姿首先由一个视线确定
     * 因此可以用一个UP向量来计算相机的旋转操作
     * 我们已知相机处于和光线垂直的平面内
     *
     * 将UP向量投影到与视线垂直的平面得到u
     * 根据视线方向w，构建一个坐标系(u,v,w)来表示相机的旋转
     * 我们通常用(0,1,0)表示UP向量，因为y轴表示图像的height轴
     *
     * 整个框架给定的坐标是真实坐标，相机的参数都是真实世界的参数
     * 图片的投影平面也是真实世界下的坐标
     *
     * 1.为了支持运动模糊效果
     * 我们让相机保存两个时间点t0,t1，从而能够
     * 在这两个时间点之间随机生成光线
     * */

  public:
    Camera()
    {
        lower_left_corner = Vector3f(-2.0, -1.0, -1.0);
        horizontal = Vector3f(4.0, 0.0, 0.0);
        vertical = Vector3f(0.0, 2.0, 0.0);
        origin = Vector3f(0.0, 0.0, 0.0);
    }
    /* 
     * vfov代表图片高的视角，调用的时候使用角度制
     * aspect代表width / height
     * lens_radius表示透镜的半径
     * focus_dist表示透镜到投影平面的距离
     * 为了景深效果，我们直接把相机从点变成一个面
     * 同时把成像平面投射到focus_dist倍距离的平面上
     */
    Camera(Vector3f look_from, Vector3f look_at, Vector3f vup, float vfov, float aspect, float aperture, float focus_dist, float t0,
           float t1)
        : time0(t0), time1(t1)
    {
        float theta = degree_to_radius(vfov);
        float half_height = tan(theta / 2.0);
        float half_width = half_height * aspect;
        lens_radius = aperture / 2.0;
        origin = look_from;
        //相机平面在真实空间中的三个方向
        w = (look_at - look_from).normalized();
        u = Vector3f::cross(w, vup).normalized();
        v = Vector3f::cross(u, w).normalized();
        lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v + w * focus_dist;
        horizontal = 2 * half_width * focus_dist * u;
        vertical = 2 * half_height * focus_dist * v;
    }

    //相机产生光线的函数
    Ray get_ray(float s, float t)
    {
        Vector3f random_point = lens_radius * random_point_on_disk();
        Vector3f offset = random_point.x() * u + random_point.y() * v;
        float r_time = time0 + get_rand() * (time1 - time0);
        return Ray(origin + offset, lower_left_corner - origin - offset + s * horizontal + t * vertical, r_time);
    }

  protected:
    Vector3f random_point_on_disk() { return 2 * Vector3f(get_rand(), get_rand(), 0) - Vector3f(1.0, 1.0, 0); }

  public:
    /*相机参数*/
    Vector3f origin;            //相机原点
    Vector3f lower_left_corner; //假象相机前是一个平面，给出平面的左下角坐标
    Vector3f horizontal;        //相机坐标下的水平方向
    Vector3f vertical;          //相机坐标下的垂直方向
    Vector3f u, v, w;

    float lens_radius;
    float time0, time1; //模拟快门打开和关闭的时间，用于运动模糊
};
#endif
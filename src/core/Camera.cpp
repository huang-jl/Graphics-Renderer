#include "Camera.hpp"

Camera::Camera()
{
    lower_left_corner = Vector3f(-2.0, -1.0, -1.0);
    horizontal = Vector3f(4.0, 0.0, 0.0);
    vertical = Vector3f(0.0, 2.0, 0.0);
    origin = Vector3f(0.0, 0.0, 0.0);
}

Camera::Camera(Vector3f look_from, Vector3f look_at, Vector3f vup, float vfov, float aspect, float aperture,
               float focus_dist, float t0, float t1)
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

Ray Camera::get_ray(float s, float t)
{
    Vector3f random_point = lens_radius * random_point_on_disk();
    Vector3f offset = random_point.x() * u + random_point.y() * v;
    float r_time = time0 + get_rand() * (time1 - time0);
    return Ray(origin + offset, lower_left_corner - origin - offset + s * horizontal + t * vertical, r_time);
}
#ifndef UTILS_HPP
#define UTILS_HPP
#include "vec3.hpp"

#include <random>
#include <memory>
using std::shared_ptr;
using std::make_shared;

#define MAX_DEPTH 50

class Ray;
class Hitable;

/******************************
 光线追踪的关键函数
    input:
        r——待求交的光线
        world——物体，是一个Hitable list
        depth——递归深度
 ******************************/
vec3 color(const Ray &r, shared_ptr<Hitable>world, int depth);

/******************************
 反射的关键函数
    input:
        v——入射光
        normal——反射光
    return:
        反射光的方向
 ******************************/
vec3 reflect(const vec3 &v, const vec3 &normal);

/******************************
 折射的关键函数
    input:
        v——入射光
        normal——法向量
        n——relative——入射光所在介质折射率/另一侧介质折射率
    output:
        refracted——折射光线
    return:
        是否能够折射
 ******************************/
bool refract(const vec3 &v, const vec3 &normal, float n_relative, vec3 &refracted);

/******************************
 反射比计算函数
    input:
        cosine——入射角的余弦
        ref_idx——折射率
    return:
        反射比，表示反射的大小（可以理解成概率）
 ******************************/
float schlick(float cosine, float ref_idx);

float get_rand();

vec3 random_in_unit_sphere();

shared_ptr<Hitable> generate_scene();

float ffmin(float a, float b);
float ffmax(float a, float b);
#endif
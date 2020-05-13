#ifndef UTILS_HPP
#define UTILS_HPP
#include <vecmath.h>

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
Vector3f color(const Ray &r, shared_ptr<Hitable>world, int depth);

/******************************
 反射的关键函数
    input:
        v——入射光
        normal——反射光
    return:
        反射光的方向
 ******************************/
Vector3f reflect(const Vector3f &v, const Vector3f &normal);

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
bool refract(const Vector3f &v, const Vector3f &normal, float n_relative, Vector3f &refracted);

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

Vector3f random_in_unit_sphere();

shared_ptr<Hitable> generate_scene();
shared_ptr<Hitable> two_sphere();//测试纹理
shared_ptr<Hitable> simple_light();//测试光源
shared_ptr<Hitable> cornell_box();  //cornell box测试厂家

float degree_to_radius(float degree);//角度制转弧度制
float ffmin(float a, float b);
float ffmax(float a, float b);
int imin(int a,int b);
int imax(int a,int b);
#endif
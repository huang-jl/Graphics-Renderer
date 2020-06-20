#ifndef UTILS_HPP
#define UTILS_HPP
#include <vecmath.h>

#include <memory>
#include <random>
#include <cfloat>
using std::make_shared;
using std::shared_ptr;

#define EPS 1e-3

class Ray;
class Hitable;

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
        cos_theta——入射角的余弦
    output:
        refracted——折射光线
    return:
        是否能够折射
 ******************************/
Vector3f refract(const Vector3f &v, const Vector3f &normal, float n_relative, float cos_theta);

/******************************
 反射比计算函数
    input:
        cosine——入射角的余弦
        ref_idx——折射率
    return:
        反射比，表示反射的大小（可以理解成概率）
 ******************************/
float schlick(float cosine, float ref_idx);

float get_frand();
int get_irand(int low, int high);   //[low, high]

Vector3f random_in_unit_sphere();
Vector3f random_on_unit_sphere();
Vector3f random_cosine_direction(); //利用p(direction)=cos(theta)/PI，生成的对半球采样
Vector3f random_to_sphere(float radius, float distance_squared);

shared_ptr<Hitable> generate_scene();
shared_ptr<Hitable> two_sphere();   //测试纹理
shared_ptr<Hitable> cornell_box(); // cornell box测试场景
shared_ptr<Hitable> random_box();

float degree_to_radian(float degree); //角度制转弧度制
float ffmin(float a, float b);
float ffmax(float a, float b);
int imin(int a, int b);
int imax(int a, int b);
bool is_nan(const Vector3f&);
#endif
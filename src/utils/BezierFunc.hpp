#ifndef BEZIER_FUNC_HPP
#define BEZIER_FUNC_HPP
#include <cassert>
#include <iostream>
#include <vector>

/****************************************
 * 主要是针对Bezier曲线曲面的辅助函数
 * 我们直接根据定义实现低次的Beizer曲面
 * 
 * vector中系数从低阶到高阶
 ****************************************/

//组合数返回C_n^i
float combination(int n, int i);

//多项式的类
struct Polynomial
{
    Polynomial(size_t n = 0);
    Polynomial(const std::vector<float> &c);
    Polynomial(const Polynomial &other);
    Polynomial(std::vector<float> &&c) noexcept;
    Polynomial &operator=(Polynomial &&other);
    Polynomial &operator=(const Polynomial &other);

    //产生导数vector
    void compute_deriv();
    //返回f(x)
    float operator()(float x) const;
    //返回f'(x)
    float derivative(float x) const;
    //从高到低删除系数为0的项，直到最高次非0
    void trim();
    //把阶数扩展到n，系数补0，主要是方便vector的[]运算符
    void extendTo(size_t n);
    Polynomial operator+(const Polynomial &other) const;
    Polynomial operator-(const Polynomial &other) const;
    Polynomial operator*(const Polynomial &other) const;
    Polynomial &operator+=(const Polynomial &other);
    Polynomial &operator-=(const Polynomial &other);
    Polynomial &operator*=(const Polynomial &other);
    float operator[](size_t n) const
    {
        assert(n >= 0);
        return n > order ? 0.f : coef[n];
    }

    static Polynomial bionomial(float a, float b, int n); //产生一个(a+bx)^n的多项式

    /*data*/
    // f(x) = c[order]x^n+c[order-1]x^n-1+...+c[1]x+c[0]
    size_t order;            //最高次的次数
    std::vector<float> coef; //系数
    std::vector<float> d;    //导数
};

Polynomial operator*(float s, const Polynomial &p);
Polynomial operator*(const Polynomial &p, float s);
std::ostream &operator<<(std::ostream &os, const Polynomial &p);

#endif
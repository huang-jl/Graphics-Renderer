#include "BezierFunc.hpp"
#include "Utils.hpp"

static int factorial[11]{1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800};

float combination(int n, int i)
{
    assert(n > 0 && n < 11 && i >= 0 && i <= n);
    return static_cast<float>(factorial[n]) / static_cast<float>(factorial[i] * factorial[n - i]);
}

Polynomial::Polynomial(size_t n) : order(n), coef(n + 1, 0.f), d(n, 0.f) {}

Polynomial::Polynomial(const std::vector<float> &c) : order(c.size() - 1), coef(c), d(order, 0.f)
{
    assert(order >= 0);
    trim();
    compute_deriv();
}

Polynomial::Polynomial(std::vector<float> &&c) noexcept : order(c.size() - 1), coef(std::move(c)), d(order, 0.f)
{
    assert(order >= 0);
    trim();
    compute_deriv();
}

Polynomial::Polynomial(const Polynomial &other) : order(other.order), coef(other.coef), d(other.d) {}

Polynomial &Polynomial::operator=(Polynomial &&other)
{
    if (this != &other)
    {
        std::move(other.coef.begin(), other.coef.end(), coef.begin());
        std::move(other.d.begin(), other.d.end(), d.begin());
        order = other.order;
    }
    return *this;
}

Polynomial &Polynomial::operator=(const Polynomial &other)
{
    if (this != &other)
    {
        coef = other.coef;
        d = other.d;
        order = other.order;
    }
    return *this;
}

void Polynomial::trim()
{
    while (order > 0 && coef.back() == 0)
    {
        coef.pop_back();
        d.pop_back();
        order--;
    }
}

void Polynomial::compute_deriv()
{
    for (int i = 0; i < order; ++i)
    {
        d[i] = coef[i + 1] * (i + 1);
    }
}

float Polynomial::operator()(float x) const
{
    float res = 0.0;
    float power_x = 1.0;
    for (int i = 0; i <= order; ++i)
    {
        res += coef[i] * power_x;
        power_x *= x;
    }
    return res;
}

float Polynomial::derivative(float x) const
{
    float res = 0.0;
    float power_x = 1.0;
    for (int i = 0; i < order; ++i)
    {
        res += power_x * d[i];
        power_x *= x;
    }
    return res;
}

void Polynomial::extendTo(size_t n)
{
    for (; order < n; ++order)
    {
        coef.push_back(0.f);
        d.push_back(0.f);
    }
    assert(coef.size() == (order + 1));
}

Polynomial Polynomial::operator+(const Polynomial &other) const
{
    Polynomial res(imax(order, other.order));
    for (int i = 0; i <= res.order; ++i)
    {
        res.coef[i] = (*this)[i] + other[i];
    }
    res.trim();
    res.compute_deriv();
    return res;
}

Polynomial Polynomial::operator-(const Polynomial &other) const
{
    Polynomial res(imax(order, other.order));
    for (int i = 0; i <= res.order; ++i)
    {
        res.coef[i] = (*this)[i] - other[i];
    }
    res.trim();
    res.compute_deriv();
    return res;
}

Polynomial Polynomial::operator*(const Polynomial &other) const
{
    Polynomial res(order + other.order);
    for (int i = 0; i <= order; ++i)
        for (int j = 0; j <= other.order; ++j)
        {
            res.coef[i + j] += (*this)[i] * other[j];
        }
    res.trim();
    res.compute_deriv();
    return res;
}
Polynomial &Polynomial::operator+=(const Polynomial &other)
{
    size_t n = imax(order, other.order);
    extendTo(n);
    for (int i = 0; i <= order; ++i)
    {
        coef[i] = (*this)[i] + other[i];
    }
    trim();
    compute_deriv();
    return *this;
}

Polynomial &Polynomial::operator-=(const Polynomial &other)
{
    size_t n = imax(order, other.order);
    extendTo(n);
    for (int i = 0; i <= order; ++i)
    {
        coef[i] = (*this)[i] - other[i];
    }
    trim();
    compute_deriv();
    return *this;
}

Polynomial &Polynomial::operator*=(const Polynomial &other)
{
    *this = *this * other;
    return *this;
}

Polynomial Polynomial::bionomial(float a, float b, int n) //产生一个(a+bx)^n的多项式, 10阶以内的Beizer曲线
{
    assert(n < 11 && n >= 0);
    if (b == 0)
        return Polynomial(std::vector<float>{powf(a, n)});
    Polynomial res(n);
    if (a == 0)
    {
        res.coef[n] = powf(b, n);
    }
    else
    {
        float a_n_i = powf(a, n), b_i = 1; // a^(n-i),b^i
        for (int i = 0; i <= n; ++i)
        {
            res.coef[i] = a_n_i * b_i * combination(n, i);
            a_n_i /= a;
            b_i *= b;
        }
    }
    res.compute_deriv();
    return res;
}

Polynomial operator*(float s, const Polynomial &p)
{
    if (s == 0)
        return Polynomial();
    size_t n = p.order;
    Polynomial res(n);
    for (int i = 0; i < n; ++i)
    {
        res.coef[i] = p.coef[i] * s;
        res.d[i] = p.d[i] * s;
    }
    res.coef[n] = p.coef[n] * s;
    return res;
}

Polynomial operator*(const Polynomial &p, float s) { return s * p; }

std::ostream &operator<<(std::ostream &os, const Polynomial &p)
{
    os << "Coef : \n";
    for (const auto &e : p.coef)
        os << e << " ";
    os << "\nDerivative : \n";
    for (const auto &e : p.d)
        os << e << " ";
    os << "\n";
    return os;
}
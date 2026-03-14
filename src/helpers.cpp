#include "helpers.hpp"


double mySqrt(double n)
{
    if (n < 0)
        throw std::domain_error("sqrt of negative number");
    if (n == 0.0) return 0.0;

    double guess = n / 2.0;
    for (int i = 0; i < 100; ++i)
    {
        double next = (guess + n / guess) / 2.0;
        if (myAbs(next - guess) < 1e-15)
            return next;
        guess = next;
    }
    return guess;
}

double myAbs(double n)
{
    return n < 0.0 ? -n : n;
}

long long myAbsLL(long long n)
{
    return n < 0 ? -n : n;
}

long long myRound(double n)
{
    return static_cast<long long>(n >= 0.0 ? n + 0.5 : n - 0.5);
}

long long myGcd(long long a, long long b)
{
    a = myAbsLL(a);
    b = myAbsLL(b);
    while (b != 0)
    {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

std::string formatFraction(double num, double den)
{
    if (den == 0.0)
        return "undefined";

    if (static_cast<long long>(num) == num &&
        static_cast<long long>(den) == den)
    {
        long long n = static_cast<long long>(num);
        long long d = static_cast<long long>(den);
        if (d < 0) { n = -n; d = -d; }
        long long g = myGcd(myAbsLL(n), d);
        n /= g;
        d /= g;
        if (d == 1)
            return std::to_string(n);
        return std::to_string(n) + "/" + std::to_string(d);
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << (num / den);
    return oss.str();
}


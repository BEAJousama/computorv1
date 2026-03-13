#include "Computor.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  MATH HELPERS
// ─────────────────────────────────────────────────────────────────────────────

static double myAbs(double n)
{
    return n < 0.0 ? -n : n;
}

static long long myAbsLL(long long n)
{
    return n < 0 ? -n : n;
}

static long long myRound(double n)
{
    return static_cast<long long>(n >= 0.0 ? n + 0.5 : n - 0.5);
}

static long long myGcd(long long a, long long b)
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

double Computor::mySqrt(double n)
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

// ─────────────────────────────────────────────────────────────────────────────
//  COMPUTOR CLASS IMPLEMENTATION
// ─────────────────────────────────────────────────────────────────────────────

Computor::Computor(const std::string &equation)
{
    _result = parse_equation(equation);
}

std::string Computor::fmtCoeff(double v)
{
    if (v == static_cast<long long>(v))
        return std::to_string(static_cast<long long>(v));

    std::ostringstream oss;
    oss << v;
    return oss.str();
}

std::string Computor::formatReduced() const
{
    return _result.reduced_form;
}

int Computor::degree() const
{
    return _result.polynomial_degree;
}

std::string Computor::formatFraction(long long num, long long den)
{
    if (den < 0) { num = -num; den = -den; }
    long long g = myGcd(myAbsLL(num), den);
    num /= g;
    den /= g;
    if (den == 1)
        return std::to_string(num);
    return std::to_string(num) + "/" + std::to_string(den);
}

void Computor::solveDegree0()
{
    double c = _result.coeffs.count(0) ? _result.coeffs.at(0) : 0.0;
    if (c == 0.0)
        std::cout << "Any real number is a solution." << std::endl;
    else
        std::cout << "No solution." << std::endl;
}

void Computor::solveDegree1()
{
    double a = _result.coeffs.count(1) ? _result.coeffs.at(1) : 0.0;
    double b = _result.coeffs.count(0) ? _result.coeffs.at(0) : 0.0;

    double numD = -b;
    double denD =  a;

    std::cout << "The solution is:" << std::endl;
    long long num = myRound(numD * 1000);
    long long den = myRound(denD * 1000);
    std::cout << formatFraction(num, den) << std::endl;
}

void Computor::solveDegree2()
{
    double a = _result.coeffs.count(2) ? _result.coeffs.at(2) : 0.0;
    double b = _result.coeffs.count(1) ? _result.coeffs.at(1) : 0.0;
    double c = _result.coeffs.count(0) ? _result.coeffs.at(0) : 0.0;

    double discriminant = b * b - 4.0 * a * c;

    if (discriminant > 0.0)
    {
        std::cout << "Discriminant is strictly positive, the two solutions are:" << std::endl;
        double sq = mySqrt(discriminant);
        double x1 = (-b + sq) / (2.0 * a);
        double x2 = (-b - sq) / (2.0 * a);

        std::cout.precision(6);
        std::cout << std::fixed << x1 << std::endl;
        std::cout << std::fixed << x2 << std::endl;
    }
    else if (discriminant == 0.0)
    {
        std::cout << "Discriminant is zero, the solution is:" << std::endl;
        double x = -b / (2.0 * a);
        std::cout << std::fixed << x << std::endl;
    }
    else
    {
        std::cout << "Discriminant is strictly negative, the two complex solutions are:" << std::endl;

        double absDisc = -discriminant;
        double sqAbs   = mySqrt(absDisc);
        double twoA    = 2.0 * a;

        long long rNum = myRound(-b   * 1000);
        long long rDen = myRound(twoA * 1000);
        long long g1   = myGcd(myAbsLL(rNum), myAbsLL(rDen));
        rNum /= g1; rDen /= g1;
        if (rDen < 0) { rNum = -rNum; rDen = -rDen; }

        long long sqInt = myRound(sqAbs);
        bool isPerfect  = myAbs((double)(sqInt * sqInt) - absDisc) < 1e-9;

        std::string realStr = formatFraction(rNum, rDen);

        if (isPerfect)
        {
            long long iNum = sqInt;
            long long iDen = myRound(twoA);
            long long g2   = myGcd(myAbsLL(iNum), myAbsLL(iDen));
            iNum /= g2; iDen /= g2;
            if (iDen < 0) { iNum = -iNum; iDen = -iDen; }

            std::string imStr = formatFraction(iNum, iDen);
            std::cout << realStr << " + " << imStr << "i" << std::endl;
            std::cout << realStr << " - " << imStr << "i" << std::endl;
        }
        else
        {
            double realVal = -b  / twoA;
            double imVal   = sqAbs / twoA;
            std::cout << std::fixed << std::setprecision(6)
                      << realVal << " + " << imVal << "i" << std::endl
                      << realVal << " - " << imVal << "i" << std::endl;
        }
    }
}

void Computor::solve()
{
    std::cout << "Reduced form: " << formatReduced() << std::endl;

    bool allZero = true;
    for (auto& [deg, coeff] : _result.coeffs)
    {
        if (coeff != 0.0) { allZero = false; break; }
    }

    if (allZero)
    {
        solveDegree0();
        return;
    }

    int deg = degree();
    std::cout << "Polynomial degree: " << deg << std::endl;

    switch (deg)
    {
        case 0: solveDegree0(); break;
        case 1: solveDegree1(); break;
        case 2: solveDegree2(); break;
        default:
            std::cout << "The polynomial degree is strictly greater than 2, I can't solve." << std::endl;
            break;
    }
}

#include "Computor.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  COMPUTOR CLASS IMPLEMENTATION
// ─────────────────────────────────────────────────────────────────────────────

Computor::Computor(const std::string &equation)
{
    _result = parse_equation(equation);
}

std::string Computor::formatReduced() const
{
    return _result.reduced_form;
}

int Computor::degree() const
{
    return _result.polynomial_degree;
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
    std::cout << formatFraction(numD, denD) << std::endl;
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

        double    sq        = mySqrt(discriminant);
        long long sqInt     = myRound(sq);
        bool      isPerfect = myAbs((double)(sqInt * sqInt) - discriminant) < 1e-9;
        double    sqNum     = isPerfect ? static_cast<double>(sqInt) : sq;

        std::cout << formatFraction(-b + sqNum, 2.0 * a) << std::endl;
        std::cout << formatFraction(-b - sqNum, 2.0 * a) << std::endl;
    }
    else if (discriminant == 0.0)
    {
        std::cout << "Discriminant is zero, the solution is:" << std::endl;
        std::cout << formatFraction(-b, 2.0 * a) << std::endl;
    }
    else
    {
        std::cout << "Discriminant is strictly negative, the two complex solutions are:" << std::endl;

        double absDisc = -discriminant;
        double sqAbs   = mySqrt(absDisc);
        double twoA    = 2.0 * a;

        long long sqInt    = myRound(sqAbs);
        bool      isPerfect = myAbs((double)(sqInt * sqInt) - absDisc) < 1e-9;
        double    imNum    = isPerfect ? static_cast<double>(sqInt) : sqAbs;

        std::string realStr = formatFraction(-b, twoA);
        std::string imStr   = formatFraction(imNum, twoA);

        std::cout << realStr << " + " << imStr << "i" << std::endl;
        std::cout << realStr << " - " << imStr << "i" << std::endl;
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

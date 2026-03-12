#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "ParserAPI.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  COMPUTOR CLASS
// ─────────────────────────────────────────────────────────────────────────────

class Computor
{
public:
    explicit Computor(const std::string &equation);
    void solve();

private:
    ParseResult _result;

    // Display
    std::string formatReduced() const;
    int degree() const;

    // Math helpers
    static double mySqrt(double n);
    static std::string fmtCoeff(double v);
    static std::string formatFraction(long long num, long long den);

    // Solvers
    void solveDegree0();
    void solveDegree1();
    void solveDegree2();
};

#pragma once
#include "ParserAPI.hpp"
#include "helpers.hpp"

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

    // Solvers
    void solveDegree0();
    void solveDegree1();
    void solveDegree2();
};

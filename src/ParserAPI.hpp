#pragma once

#include <string>
#include <map>

// ─────────────────────────────────────────────────────────────────────────────
//  PARSE RESULT
// ─────────────────────────────────────────────────────────────────────────────

struct ParseResult {
    std::map<int, double> coeffs;   // degree -> coefficient, reduced (LHS - RHS = 0)
    int polynomial_degree;
    std::string reduced_form;
};

// ─────────────────────────────────────────────────────────────────────────────
//  PUBLIC API
// ─────────────────────────────────────────────────────────────────────────────

ParseResult parse_equation(const std::string& input);

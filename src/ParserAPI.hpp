#pragma once
#include "Lexer.hpp"
#include "Parser.hpp"
#include "helpers.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  PARSE RESULT
// ─────────────────────────────────────────────────────────────────────────────

struct ParseResult {
    std::map<int, double> coeffs;
    int polynomial_degree;
    std::string reduced_form;
};

// ─────────────────────────────────────────────────────────────────────────────
//  PUBLIC API
// ─────────────────────────────────────────────────────────────────────────────

ParseResult parse_equation(const std::string& input);
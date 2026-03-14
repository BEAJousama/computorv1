#pragma once
#include "Lexer.hpp"
#include "helpers.hpp"
#include <map>

// ─────────────────────────────────────────────────────────────────────────────
//  MONOMIAL
// ─────────────────────────────────────────────────────────────────────────────

struct Monomial {
    double coeff;
    int    degree;
};

// ─────────────────────────────────────────────────────────────────────────────
//  PARSER
// ─────────────────────────────────────────────────────────────────────────────

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::map<int, double> parse();

private:
    const std::vector<Token>& tokens_;
    size_t                    pos_;

    const Token& cur() const;
    Token consume();
    bool check(TokenType t) const;
    bool match(TokenType t);
    void expect(TokenType t, const std::string& what);
    int parse_exponent();
    std::map<int, double> parse_expression();
    std::vector<Monomial> parse_term(double outer_sign);
    std::vector<Monomial> parse_factor(double sign);
};

#include "Parser.hpp"
#include "Lexer.hpp"
#include <stdexcept>
#include <cmath>

static double myAbs(double n)
{
    return n < 0.0 ? -n : n;
}

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), pos_(0) {}

const Token& Parser::cur() const {
    return tokens_[pos_];
}

Token Parser::consume() {
    return tokens_[pos_++];
}

bool Parser::check(TokenType t) const {
    return cur().type == t;
}

bool Parser::match(TokenType t) {
    if (check(t)) { ++pos_; return true; }
    return false;
}

void Parser::expect(TokenType t, const std::string& what) {
    if (!check(t))
        throw std::runtime_error("Expected " + what + " but got '" + cur().raw + "'");
    ++pos_;
}

int Parser::parse_exponent() {
    if (!check(TokenType::NUMBER))
        throw std::runtime_error("Expected integer exponent after '^'");
    double v = consume().value;
    if (v < 0 || v != static_cast<long long>(v))
        throw std::runtime_error("Exponent must be non-negative integer");
    return static_cast<int>(v);
}

std::map<int, double> Parser::parse_expression() {
    std::map<int, double> result;
    auto monomials = parse_term(+1.0);
    for (auto& m : monomials) result[m.degree] += m.coeff;

    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        double sign = (consume().type == TokenType::PLUS) ? +1.0 : -1.0;
        for (auto& m : parse_term(sign))
            result[m.degree] += m.coeff;
    }
    return result;
}

std::vector<Monomial> Parser::parse_term(double outer_sign) {
    double sign = outer_sign;
    if (check(TokenType::PLUS)) { ++pos_; }
    else if (check(TokenType::MINUS)) { ++pos_; sign = -sign; }
    return parse_factor(sign);
}

std::vector<Monomial> Parser::parse_factor(double sign) {
    if (check(TokenType::NUMBER)) {
        double coeff = sign * consume().value;
        bool has_star = match(TokenType::STAR);

        if (check(TokenType::X)) {
            ++pos_;
            int degree = 1;
            if (match(TokenType::CARET))
                degree = parse_exponent();
            return {{ coeff, degree }};
        } else if (has_star) {
            throw std::runtime_error("Expected 'X' after '*'");
        } else {
            return {{ coeff, 0 }};
        }
    }
    else if (check(TokenType::X)) {
        ++pos_;
        int degree = 1;
        if (match(TokenType::CARET))
            degree = parse_exponent();
        return {{ sign * 1.0, degree }};
    }
    else {
        throw std::runtime_error("Unexpected token while parsing term");
    }
}

std::map<int, double> Parser::parse() {
    auto lhs = parse_expression();
    expect(TokenType::EQUAL, "'='");
    auto rhs = parse_expression();
    expect(TokenType::END, "end of input");

    for (auto& [deg, coeff] : rhs)
        lhs[deg] -= coeff;

    std::map<int, double> result;
    for (auto& [deg, coeff] : lhs)
        if (myAbs(coeff) > 1e-12)
            result[deg] = coeff;

    return result;
}

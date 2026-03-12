#include "Lexer.hpp"
#include <cctype>

Lexer::Lexer(std::string src) : src_(std::move(src)), pos_(0) {}

void Lexer::skip_ws() {
    while (pos_ < src_.size() && std::isspace(src_[pos_])) ++pos_;
}

Token Lexer::read_number() {
    size_t start = pos_;
    bool has_dot = false;
    while (pos_ < src_.size() && (std::isdigit(src_[pos_]) || src_[pos_] == '.')) {
        if (src_[pos_] == '.') {
            if (has_dot) throw std::runtime_error("Malformed number");
            has_dot = true;
        }
        ++pos_;
    }
    std::string raw = src_.substr(start, pos_ - start);
    if (raw == ".") throw std::runtime_error("Lone '.'");
    return {TokenType::NUMBER, std::stod(raw), raw};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos_ < src_.size()) {
        skip_ws();
        if (pos_ >= src_.size()) break;

        char c = src_[pos_];

        if (c == '+') { tokens.push_back({TokenType::PLUS,  0, "+"}); ++pos_; }
        else if (c == '-') { tokens.push_back({TokenType::MINUS, 0, "-"}); ++pos_; }
        else if (c == '*') { tokens.push_back({TokenType::STAR,  0, "*"}); ++pos_; }
        else if (c == '^') { tokens.push_back({TokenType::CARET, 0, "^"}); ++pos_; }
        else if (c == '=') { tokens.push_back({TokenType::EQUAL, 0, "="}); ++pos_; }
        else if (c == 'X' || c == 'x') { tokens.push_back({TokenType::X, 0, "X"}); ++pos_; }
        else if (std::isdigit(c) || c == '.') {
            tokens.push_back(read_number());
        }
        else {
            throw std::runtime_error(std::string("Unexpected character '") + c + "'");
        }
    }
    tokens.push_back({TokenType::END, 0, ""});
    return tokens;
}

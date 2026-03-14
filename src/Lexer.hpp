#pragma once
#include <vector>


// ─────────────────────────────────────────────────────────────────────────────
//  TOKEN
// ─────────────────────────────────────────────────────────────────────────────

enum class TokenType {
    NUMBER,
    X,
    CARET,
    STAR,
    PLUS,
    MINUS,
    EQUAL,
    END,
};

struct Token {
    TokenType type;
    double    value;
    std::string raw;
};

// ─────────────────────────────────────────────────────────────────────────────
//  LEXER
// ─────────────────────────────────────────────────────────────────────────────

class Lexer {
public:
    explicit Lexer(std::string src);

    std::vector<Token> tokenize();

private:
    std::string src_;
    size_t      pos_;

    void skip_ws();
    Token read_number();
};

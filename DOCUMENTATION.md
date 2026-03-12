# ComputorV1 — Complete Documentation

A line-by-line explanation of the entire codebase, from input to solution.

---

## Table of Contents

1. [Project Structure](#1-project-structure)
2. [main.cpp — Entry Point](#2-maincpp--entry-point)
3. [Lexer (Lexer.hpp / Lexer.cpp)](#3-lexer-lexerhpp--lexercpp)
4. [Parser (Parser.hpp / Parser.cpp)](#4-parser-parserhpp--parsercpp)
5. [ParserAPI (ParserAPI.hpp / ParserAPI.cpp)](#5-parserapi-parserapihpp--parserapicpp)
6. [Computor (Computor.hpp / Computor.cpp)](#6-computor-computorhpp--computorcpp)
7. [Math Helpers Explained](#7-math-helpers-explained)
8. [Flow Summary](#8-flow-summary)

---

## 1. Project Structure

```
src/
├── main.cpp       ← Program entry point
├── Lexer.hpp     ← Token type definitions
├── Lexer.cpp     ← Lexer implementation
├── Parser.hpp    ← Parser class definition
├── Parser.cpp    ← Parser implementation
├── ParserAPI.hpp ← ParseResult struct + parse_equation() declaration
├── ParserAPI.cpp ← parse_equation() implementation + buildReducedForm()
├── Computor.hpp  ← Computor class declaration
└── Computor.cpp  ← Computor implementation + solvers + math helpers
```

---

## 2. main.cpp — Entry Point

```cpp
#include "Computor.hpp"
#include <iostream>
#include <exception>

int main(int argc, char **argv)
{
    std::string equation;

    // If command-line arguments exist, concatenate them
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
            equation += argv[i];   // no spaces added between args
    }
    else
    {
        // Otherwise read from stdin
        std::cout << "Enter the equation:" << std::endl;
        std::getline(std::cin, equation);
    }

    try
    {
        Computor computor(equation);
        computor.solve();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Line-by-line:**

- **Line 1:** Include the Computor header to use the class.
- **Line 2-3:** Include iostream (for std::cout/cerr) and exception (for std::exception).
- **Line 5:** `main` takes `argc` (argument count) and `argv` (argument vector).
- **Line 7:** Empty string to hold the equation.
- **Line 9-13:** If arguments were passed (`argc > 1`), concatenate all arguments into one string. `argv[0]` is the program name, so we start at `argv[1]`.
- **Line 14-18:** If no arguments, prompt user and read a line from stdin using `std::getline` (preserves spaces).
- **Line 20-30:** Wrap everything in try/catch. If any exception is thrown during parsing or solving, print the error message to `std::cerr` (error stream) and return 1 (error exit code).

---

## 3. Lexer (Lexer.hpp / Lexer.cpp)

The Lexer converts a raw string like `"5 * X^2 + 3 * X = 4"` into a list of **tokens**.

### Lexer.hpp

```cpp
#pragma once

#include <string>
#include <vector>

// Token types that the lexer can produce
enum class TokenType {
    NUMBER,   // 3, 3.14, .5, 42
    X,        // X or x (the variable)
    CARET,    // ^
    STAR,     // *
    PLUS,     // +
    MINUS,    // -
    EQUAL,    // =
    END,      // end of input ( A tokensentinel)
};

//: type + numeric value (for NUMBER) + raw string (for error messages)
struct Token {
    TokenType type;
    double    value;    // only meaningful for NUMBER tokens
    std::string raw;   // original text
};
```

```cpp
class Lexer {
public:
    explicit Lexer(std::string src);           // constructor takes the equation string
    std::vector<Token> tokenize();             // main method: returns list of tokens

private:
    std::string src_;   // the input string
    size_t      pos_;   // current position while scanning

    void skip_ws();      // helper: skip whitespace
    Token read_number(); // helper: read a numeric token
};
```

### Lexer.cpp

```cpp
#include "Lexer.hpp"
#include <cctype>

Lexer::Lexer(std::string src) : src_(std::move(src)), pos_(0) {}
```

- Constructor takes string by value, uses `std::move` to avoid copying. Initializes `pos_` to 0.

```cpp
void Lexer::skip_ws() {
    while (pos_ < src_.size() && std::isspace(src_[pos_])) ++pos_;
}
```

- Skip all whitespace characters. `std::isspace` checks for space, tab, newline, etc.

```cpp
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
```

- Reads a number: digits and optionally one decimal point.
- `has_dot` ensures we don't have multiple dots (e.g., "3..5").
- At the end, convert the string to `double` using `std::stod`.
- If only a single dot was read (". "), that's an error.

```cpp
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
```

- Main tokenization loop: look at each character, produce the appropriate token.
- Skip whitespace at the start of each iteration.
- For each character:
  - `+` → PLUS token
  - `-` → MINUS token
  - `*` → STAR token
  - `^` → CARET token
  - `=` → EQUAL token
  - `X` or `x` → X token
  - digit or `.` → read the full number
  - anything else → throw error
- At the end, add an END token as a sentinel.

---

## 4. Parser (Parser.hpp / Parser.cpp)

The Parser takes tokens and builds a polynomial (map of degree → coefficient).

### Parser.hpp

```cpp
struct Monomial {
    double coeff;
    int    degree;
};
```

- Simple struct to hold one term: coefficient and exponent.

```cpp
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::map<int, double> parse();   // returns coefficients map

private:
    const std::vector<Token>& tokens_;  // reference to token list
    size_t                    pos_;      // current position in token list

    // Helper methods
    const Token& cur() const;              // peek at current token
    Token consume();                        // get current token and advance
    bool check(TokenType t) const;          // is current token of type t?
    bool match(TokenType t);                // if current is t, consume and return true
    void expect(TokenType t, const std::string& what);  // require token type or throw

    // Grammar rules
    int parse_exponent();                                    // parse integer after ^
    std::map<int, double> parse_expression();                 // expression = term ((+|-) term)*
    std::vector<Monomial> parse_term(double outer_sign);     // term = [+-] factor
    std::vector<Monomial> parse_factor(double sign);        // factor = number [*] X [^ exp]
};
```

### Parser.cpp — Helper Methods

```cpp
const Token& Parser::cur() const {
    return tokens_[pos_];
}
```

- Peek at current token without advancing.

```cpp
Token Parser::consume() {
    return tokens_[pos_++];
}
```

- Get current token and advance to next position.

```cpp
bool Parser::check(TokenType t) const {
    return cur().type == t;
}
```

- Check if current token is of a specific type.

```cpp
bool Parser::match(TokenType t) {
    if (check(t)) { ++pos_; return true; }
    return false;
}
```

- If current token matches, consume it and return true. Otherwise return false without consuming.

```cpp
void Parser::expect(TokenType t, const std::string& what) {
    if (!check(t))
        throw std::runtime_error("Expected " + what + " but got '" + cur().raw + "'");
    ++pos_;
}
```

- Require a specific token type. If not present, throw an error with a helpful message.

### Parser.cpp — Grammar Rules

```cpp
int Parser::parse_exponent() {
    if (!check(TokenType::NUMBER))
        throw std::runtime_error("Expected integer exponent after '^'");
    double v = consume().value;
    if (v < 0 || v != static_cast<long long>(v))
        throw std::runtime_error("Exponent must be non-negative integer");
    return static_cast<int>(v);
}
```

- Parse an exponent: must be a non-negative whole number. Convert double to int.

```cpp
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
```

- **Expression** = term ((PLUS | MINUS) term)*
- First parse a term with positive sign.
- Then, as long as we see + or -, parse another term with that sign.
- Accumulate all monomials into the result map (summing coefficients for same degree).

```cpp
std::vector<Monomial> Parser::parse_term(double outer_sign) {
    double sign = outer_sign;
    if (check(TokenType::PLUS)) { ++pos_; }
    else if (check(TokenType::MINUS)) { ++pos_; sign = -sign; }
    return parse_factor(sign);
}
```

- **Term** = [PLUS | MINUS] factor
- Handle leading unary + or - (e.g., "-5 * X^2"). The sign gets passed to parse_factor.

```cpp
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
            return {{ coeff, 0 }};  // constant term (X^0)
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
```

- **Factor** has three cases:
  1. **NUMBER**: read coefficient
     - If followed by X: parse X with optional ^exponent, return `{coeff, degree}`
     - If * but no X: error
     - If neither: it's a constant, return `{coeff, 0}`
  2. **X**: no coefficient given, default to 1
     - Parse optional ^exponent, return `{sign * 1.0, degree}`
  3. **Anything else**: error

```cpp
std::map<int, double> Parser::parse() {
    auto lhs = parse_expression();
    expect(TokenType::EQUAL, "'='");
    auto rhs = parse_expression();
    expect(TokenType::END, "end of input");

    // Reduction: move everything to left side (LHS - RHS = 0)
    for (auto& [deg, coeff] : rhs)
        lhs[deg] -= coeff;

    // Remove near-zero coefficients (floating point tolerance)
    std::map<int, double> result;
    for (auto& [deg, coeff] : lhs)
        if (myAbs(coeff) > 1e-12)
            result[deg] = coeff;

    return result;
}
```

- **parse()** — top-level method:
  1. Parse left-hand side expression
  2. Expect and consume `=`
  3. Parse right-hand side expression
  4. Expect END token (nothing after)
  5. Subtract RHS from LHS to get reduced form
  6. Filter out coefficients that are effectively zero (due to floating point)

---

## 5. ParserAPI (ParserAPI.hpp / ParserAPI.cpp)

```cpp
struct ParseResult {
    std::map<int, double> coeffs;      // degree -> coefficient, reduced
    int polynomial_degree;                // highest degree with non-zero coeff
    std::string reduced_form;            // formatted string like "4 * X^0 + 3 * X^1 = 0"
};
```

- Structure to hold the result of parsing.

```cpp
static std::string buildReducedForm(const std::map<int, double>& coeffs) {
    if (coeffs.empty()) return "0 * X^0 = 0";

    std::ostringstream oss;
    bool first = true;

    for (auto& [deg, coeff] : coeffs) {
        double abs_c = myAbs(coeff);
        std::string sign_str;
        if (first) {
            sign_str = (coeff < 0) ? "-" : "";
        } else {
            sign_str = (coeff < 0) ? " - " : " + ";
        }
        oss << sign_str;

        if (abs_c == static_cast<long long>(abs_c))
            oss << static_cast<long long>(abs_c);
        else
            oss << abs_c;

        oss << " * X^" << deg;
        first = false;
    }
    oss << " = 0";
    return oss.str();
}
```

- Builds the reduced form string:
  - Iterate over coeffs (sorted by degree)
  - First term: use "-" for negative, nothing for positive
  - Later terms: use " - " or " + "
  - If coefficient is a whole number, print without decimal (.0)
  - End with " = 0"

```cpp
ParseResult parse_equation(const std::string& input) {
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto coeffs = parser.parse();

    int degree = 0;
    if (!coeffs.empty())
        degree = coeffs.rbegin()->first;  // highest key in map

    ParseResult result;
    result.coeffs = coeffs;
    result.polynomial_degree = degree;
    result.reduced_form = buildReducedForm(coeffs);
    return result;
}
```

- **parse_equation()** — the public API:
  1. Create lexer, tokenize input
  2. Create parser, parse to get coefficients map
  3. Find highest degree (last key in map using `rbegin()`)
  4. Build result struct and return

---

## 6. Computor (Computor.hpp / Computor.cpp)

```cpp
class Computor
{
public:
    explicit Computor(const std::string &equation);
    void solve();

private:
    ParseResult _result;  // stores parsed polynomial

    std::string formatReduced() const;
    int degree() const;

    // Math helpers (no cmath)
    static double mySqrt(double n);
    static std::string fmtCoeff(double v);
    static std::string formatFraction(long long num, long long den);

    // Solvers
    void solveDegree0();
    void solveDegree1();
    void solveDegree2();
};
```

### Math Helpers

```cpp
static double myAbs(double n) {
    return n < 0.0 ? -n : n;
}
```

- Absolute value for doubles.

```cpp
static long long myAbsLL(long long n) {
    return n < 0 ? -n : n;
}
```

- Absolute value for long long.

```cpp
static long long myRound(double n) {
    return static_cast<long long>(n >= 0.0 ? n + 0.5 : n - 0.5);
}
```

- Round to nearest integer:
  - For positive: add 0.5 then truncate
  - For negative: subtract 0.5 then truncate (e.g., -3.7 - 0.5 = -4.2 → -4)

```cpp
static long long myGcd(long long a, long long b) {
    a = myAbsLL(a);
    b = myAbsLL(b);
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}
```

- Greatest Common Divisor using Euclidean algorithm:
  - `GCD(a, b) = GCD(b, a % b)` repeatedly until b becomes 0.

```cpp
double Computor::mySqrt(double n) {
    if (n < 0)
        throw std::domain_error("sqrt of negative number");
    if (n == 0.0) return 0.0;

    double guess = n / 2.0;
    for (int i = 0; i < 100; ++i) {
        double next = (guess + n / guess) / 2.0;
        if (myAbs(next - guess) < 1e-15)
            return next;
        guess = next;
    }
    return guess;
}
```

- Square root using **Newton-Raphson** method:
  - Start with guess = n / 2
  - Each iteration: better_guess = (guess + n/guess) / 2
  - Stop when improvement is very small (< 1e-15)
  - Maximum 100 iterations (usually converges in < 10)

### Computor Constructor

```cpp
Computor::Computor(const std::string &equation) {
    _result = parse_equation(equation);
}
```

- Simply call the parser API to get the parsed polynomial.

### Solvers

```cpp
void Computor::solveDegree0() {
    double c = _result.coeffs.count(0) ? _result.coeffs.at(0) : 0.0;
    if (c == 0.0)
        std::cout << "Any real number is a solution." << std::endl;
    else
        std::cout << "No solution." << std::endl;
}
```

- Degree 0: constant equation `c = 0`
  - If c = 0: infinite solutions
  - If c ≠ 0: no solution

```cpp
void Computor::solveDegree1() {
    double a = _result.coeffs.count(1) ? _result.coeffs.at(1) : 0.0;
    double b = _result.coeffs.count(0) ? _result.coeffs.at(0) : 0.0;

    // a*X + b = 0  →  X = -b / a
    double numD = -b;
    double denD =  a;

    // Convert to integer fraction (multiply by 1000, round, reduce)
    long long num = myRound(numD * 1000);
    long long den = myRound(denD * 1000);
    long long g   = myGcd(myAbsLL(num), myAbsLL(den));
    num /= g;
    den /= g;
    if (den < 0) { num = -num; den = -den; }

    std::cout << "The solution is:" << std::endl;
    if (den == 1)
        std::cout << num << std::endl;
    else
        std::cout << num << "/" << den << std::endl;
}
```

- Degree 1: `a*X + b = 0` → `X = -b/a`
- Display as reduced fraction by scaling to integers.

```cpp
void Computor::solveDegree2() {
    double a = _result.coeffs.count(2) ? _result.coeffs.at(2) : 0.0;
    double b = _result.coeffs.count(1) ? _result.coeffs.at(1) : 0.0;
    double c = _result.coeffs.count(0) ? _result.coeffs.at(0) : 0.0;

    double discriminant = b * b - 4.0 * a * c;

    if (discriminant > 0.0) {
        // Two real roots: (-b ± √Δ) / 2a
        double sq = mySqrt(discriminant);
        double x1 = (-b + sq) / (2.0 * a);
        double x2 = (-b - sq) / (2.0 * a);
        std::cout << "Discriminant is strictly positive, the two solutions are:" << std::endl;
        std::cout.precision(6);
        std::cout << std::fixed << x1 << std::endl;
        std::cout << std::fixed << x2 << std::endl;
    }
    else if (discriminant == 0.0) {
        // One repeated root: -b / 2a
        double x = -b / (2.0 * a);
        std::cout << "Discriminant is zero, the solution is:" << std::endl;
        std::cout << std::fixed << x << std::endl;
    }
    else {
        // Complex roots: -b/(2a) ± i*√(-Δ)/(2a)
        double absDisc = -discriminant;
        double sqAbs   = mySqrt(absDisc);
        double twoA    = 2.0 * a;

        // Real part as fraction
        long long rNum = myRound(-b * 1000);
        long long rDen = myRound(twoA * 1000);
        long long g1   = myGcd(myAbsLL(rNum), myAbsLL(rDen));
        rNum /= g1; rDen /= g1;
        if (rDen < 0) { rNum = -rNum; rDen = -rDen; }

        // Imaginary part: check if perfect square
        long long sqInt = myRound(sqAbs);
        bool isPerfect = myAbs((double)(sqInt * sqInt) - absDisc) < 1e-9;

        std::string realStr = formatFraction(rNum, rDen);

        if (isPerfect) {
            long long iNum = sqInt;
            long long iDen = myRound(twoA);
            long long g2 = myGcd(myAbsLL(iNum), myAbsLL(iDen));
            iNum /= g2; iDen /= g2;
            if (iDen < 0) { iNum = -iNum; iDen = -iDen; }
            std::string imStr = formatFraction(iNum, iDen);
            std::cout << realStr << " + " << imStr << "i" << std::endl;
            std::cout << realStr << " - " << imStr << "i" << std::endl;
        } else {
            double realVal = -b / twoA;
            double imVal = sqAbs / twoA;
            std::cout << std::fixed << std::setprecision(6)
                      << realVal << " + " << imVal << "i" << std::endl
                      << realVal << " - " << imVal << "i" << std::endl;
        }
    }
}
```

- Degree 2: quadratic formula
  - Compute discriminant Δ = b² - 4ac
  - Δ > 0: two distinct real roots
  - Δ = 0: one repeated real root
  - Δ < 0: two complex conjugate roots
  - Try to display complex parts as fractions when possible (perfect square check)

```cpp
void Computor::solve() {
    std::cout << "Reduced form: " << formatReduced() << std::endl;

    // Check if all coefficients are zero
    bool allZero = true;
    for (auto& [deg, coeff] : _result.coeffs) {
        if (coeff != 0.0) { allZero = false; break; }
    }

    if (allZero) {
        solveDegree0();
        return;
    }

    int deg = degree();
    std::cout << "Polynomial degree: " << deg << std::endl;

    switch (deg) {
        case 0: solveDegree0(); break;
        case 1: solveDegree1(); break;
        case 2: solveDegree2(); break;
        default:
            std::cout << "The polynomial degree is strictly greater than 2, I can't solve." << std::endl;
            break;
    }
}
```

- Main entry point:
  1. Print reduced form
  2. Check if all zero (infinite solutions)
  3. Get polynomial degree
  4. Dispatch to appropriate solver
  5. If degree > 2, refuse to solve

---

## 7. Math Helpers Explained

| Function | Purpose | Algorithm |
|----------|---------|-----------|
| `myAbs` | Absolute value | Ternary: `n < 0 ? -n : n` |
| `myAbsLL` | Absolute value for long long | Same as above |
| `myRound` | Round to nearest integer | Add 0.5 (or subtract for negatives), then truncate |
| `myGcd` | Greatest Common Divisor | Euclidean algorithm: `GCD(a,b) = GCD(b, a%b)` |
| `mySqrt` | Square root | Newton-Raphson: `x_new = (x + n/x) / 2` |

---

## 8. Flow Summary

```
User input: "5 * X^2 = 20"
    │
    ▼
main() reads string
    │
    ▼
Computor(equation) constructor
    │
    ▼
parse_equation(string)
    │
    ├─► Lexer.tokenize() → [NUMBER(5), STAR, X, CARET, NUMBER(2), EQUAL, NUMBER(20), END]
    │
    ├─► Parser.parse()
    │     │
    │     ├─► parse_expression() for LHS
    │     │     └─► parse_term() → parse_factor() → returns {5, 2}
    │     │
    │     ├─► expect(EQUAL)
    │     │
    │     ├─► parse_expression() for RHS
    │     │     └─► parse_term() → parse_factor() → returns {20, 0}
    │     │
    │     └─► reduce: LHS - RHS → { {0, -20}, {2, 5} }
    │
    ├─► buildReducedForm() → "-20 * X^0 + 5 * X^2 = 0"
    │
    └─► ParseResult { coeffs = {...}, degree = 2, reduced_form = "..." }
    │
    ▼
solve()
    │
    ├─► Print reduced form
    ├─► degree() = 2
    ├─► solveDegree2()
    │     │
    │     ├─► discriminant = b² - 4ac = 0 - 4*5*(-20) = 400
    │     ├─► Δ > 0, compute roots
    │     └─► Print: "2.000000" and "-2.000000"
    │
    ▼
Output printed to console
```

---

End of documentation.

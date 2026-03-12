#include "ParserAPI.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <sstream>
#include <cmath>

static double myAbs(double n)
{
    return n < 0.0 ? -n : n;
}

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

ParseResult parse_equation(const std::string& input) {
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto coeffs = parser.parse();

    int degree = 0;
    if (!coeffs.empty())
        degree = coeffs.rbegin()->first;

    ParseResult result;
    result.coeffs = coeffs;
    result.polynomial_degree = degree;
    result.reduced_form = buildReducedForm(coeffs);
    return result;
}

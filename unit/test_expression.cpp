// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/expressionNEW.h"
#include "expression/reference.h"
#include "expression/variable.h"
#include <fmt/format.h>
#include <string_view>
#include <tuple>
#include <vector>

int main(int args, char **argv)
{
    // Test ANTLR-based Expression parser

    // Set up test data:  <"name", "expression", "expectedValue", "shouldFail">
    RefList<ExpressionVariable> variables;
    ExpressionVariable a(1.0);
    a.setName("a");
    ExpressionVariable bee(100.0);
    bee.setName("bee");
    variables.append(&a);
    variables.append(&bee);
    std::vector<std::tuple<std::string_view, std::string_view, ExpressionValue, bool>> tests = {
        {"Basic integer math 1", "1-(3*2)", 1 - (3 * 2), false},
        {"Basic integer math 2", "1-3*2", 1 - 3 * 2, false},
        {"Basic integer math 3", "242/15", 242 / 15, false},
        {"Basic floating point math", "1.0+24*3.4", 1.0 + 24 * 3.4, false},
        {"Parentheses", "(9.231-4*89.4)/76.92+7", (9.231 - 4 * 89.4) / 76.92 + 7, false},
        {"Unary minus", "-(6.5+7)", -(6.5 + 7), false},
        {"Power", "-3.0^3", pow(-3.0, 3), false},
        {"Bad operators 1", "2++7", 0, true},
        {"Bad operators 2", "2+7*", 0, true},
        {"Legit odd operators 1", "2+-7", 2 + -7, false},
        {"Legit odd operators 2", "2--7", 2 - (-7), false},
        {"Functions 1", "cos(45)", cos(M_PI_4), false},
        {"Functions 2", "exp(cos(45))", exp(cos(M_PI_4)), false},
        {"Functions 3", "sqrt(sin(78.9)*cos(45))", sqrt(sin(78.9 * M_PI / 180.0) * cos(M_PI_4)), false},
        {"Bad function args 1", "atan()", 0, true},
        {"Bad function args 2", "ln(0.0, 1.0)", 0, true},
        {"Variables 1", "a/5", a.value().asDouble() / 5, false},
        {"Variables 2", "a + sqrt(bee)", a.value().asDouble() + sqrt(bee.value().asDouble()), false},
        {"Bad name", "1.8*wasp", 0, true}};

    const auto threshold = 1.0e-10;

    for (auto test : tests)
    {
        auto title = std::get<0>(test);
        auto expr = std::get<1>(test);
        auto expectedResult = std::get<2>(test);
        auto shouldFail = std::get<3>(test);

        fmt::print("Evaluating: {}\n", title);
        ExpressionNEW expression;
        auto generationResult = expression.create(expr, variables);
        if (shouldFail)
        {
            if (generationResult)
                throw(std::runtime_error(fmt::format("Expression '{}' parsed correctly when it shouldn't have.", expr)));
            else
                continue;
        }
        else if (!generationResult)
            throw(std::runtime_error(fmt::format("Expression '{}' did not parse correctly.", expr)));

        // Evaluate the expression
        auto optResult = expression.evaluate();
        if (!optResult)
            throw(std::runtime_error(fmt::format("Expression '{}' failed to evaluate.", expr)));

        auto result = (*optResult);

        // Check result type
        if (expectedResult.type() != result.type())
            throw(std::runtime_error(
                fmt::format("Expression '{}' failed - actual result is not of equivalent type to expected result.", expr)));

        // Compare result values
        if (expectedResult.isInteger() && (expectedResult.asInteger() != result.asInteger()))
            throw(std::runtime_error(
                fmt::format("Expression '{}' failed - actual result '{}' does not match expected result '{}'.", expr,
                            result.asInteger(), expectedResult.asInteger())));
        else if (fabs(result.asDouble() - expectedResult.asDouble()) > threshold)
            throw(std::runtime_error(fmt::format(
                "Expression '{}' failed - actual result '{}' does not match expected result '{}' (is beyond threshold).", expr,
                result.asDouble(), expectedResult.asDouble())));
    }

    // Success
    exit(0);
}

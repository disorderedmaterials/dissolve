// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include <cmath>

#include "expression/expression.h"
#include "expression/reference.h"
#include "expression/variable.h"
#include "math/mathFunc.h"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <string_view>
#include <vector>

namespace UnitTest
{

class ExpressionTest : public ::testing::Test
{
    public:
    ExpressionTest() = default;

    protected:
    void SetUp() override
    {
        variables.emplace_back(std::make_shared<ExpressionVariable>("a", ExpressionValue(1.0)));
        variables.emplace_back(std::make_shared<ExpressionVariable>("bee", ExpressionValue(100.0)));
    }

    private:
    void checkExpr(std::string_view expr, const ExpressionValue &val, bool shouldFail)
    {
        auto generationResult = expression.create(expr, variables);
        ASSERT_NE(bool(generationResult), shouldFail);
        if (shouldFail)
            return;
        auto optResult = expression.evaluate();
        ASSERT_TRUE(optResult);
        auto result = (*optResult);
        if (val.type() != result.type())
            throw std::runtime_error(fmt::format("Expected: {}, Got: {} for expression {} and value {}", int(val.type()),
                                                 int(result.type()), expr, val.asString()));
        ASSERT_TRUE(result.type() == val.type());
        switch (val.type())
        {
            case (ExpressionValue::ValueType::Double):
                EXPECT_DOUBLE_EQ(result.asDouble(), val.asDouble());
                break;
            case (ExpressionValue::ValueType::Integer):
                EXPECT_EQ(result.asInteger(), val.asInteger());
                break;
            case (ExpressionValue::ValueType::Bool):
                EXPECT_EQ(result.asBool(), val.asBool());
                break;
        }
    }

    protected:
    void exprTest(std::string_view expr, const ExpressionValue &val, bool shouldFail)
    {
        // Test expression
        checkExpr(expr, val, shouldFail);
        if (shouldFail)
            return;

        // Regenerate expression string from nodes and re-test
        EXPECT_EQ(expr, expression.expressionString());
        checkExpr(expression.expressionString(), val, shouldFail);
    }

    std::vector<std::shared_ptr<ExpressionVariable>> variables;
    Expression expression;
};

TEST_F(ExpressionTest, BasicIntegerMath)
{
    exprTest("1-(3*2)", ExpressionValue(1 - (3 * 2)), false);
    exprTest("1-3*2", ExpressionValue(1 - 3 * 2), false);
    exprTest("242/15", ExpressionValue(242 / 15), false);
    exprTest("-(6.5+7)", ExpressionValue(-(6.5 + 7)), false);
    exprTest("2+-7", ExpressionValue(2 + -7), false);
    exprTest("2--7", ExpressionValue(2 - (-7)), false);
    exprTest("2++7", ExpressionValue(0), true);
    exprTest("2+7*", ExpressionValue(0), true);
}

TEST_F(ExpressionTest, BasicFloatMath)
{
    exprTest("1+24*3.4", ExpressionValue(1.0 + 24 * 3.4), false);
    exprTest("(9.231-4*89.4)/76.92+7", ExpressionValue((9.231 - 4 * 89.4) / 76.92 + 7), false);
    exprTest("-3^3", ExpressionValue(DissolveMath::power(-3, 3)), false);
    exprTest("cos(toRad(45))", ExpressionValue(cos(M_PI_4)), false);
    exprTest("exp(cos(toRad(45)))", ExpressionValue(exp(cos(M_PI_4))), false);
    exprTest("sqrt(sin(toRad(78.9))*cos(toRad(45)))", ExpressionValue(sqrt(sin(78.9 * M_PI / 180.0) * cos(M_PI_4))), false);
    exprTest("atan()", ExpressionValue(0), true);
    exprTest("ln(0.0, 1.0)", ExpressionValue(0), true);
}

TEST_F(ExpressionTest, Variables)
{
    auto a = variables[0];
    auto bee = variables[1];
    exprTest("a/5", ExpressionValue(a->value().asDouble() / 5), false);
    exprTest("a+sqrt(bee)", ExpressionValue(a->value().asDouble() + sqrt(bee->value().asDouble())), false);
    exprTest("1.8*wasp", ExpressionValue(0), true);
}

TEST_F(ExpressionTest, Boolean)
{
    exprTest("0<1", ExpressionValue(true), false);
    exprTest("1>0", ExpressionValue(true), false);
    exprTest("1==1", ExpressionValue(true), false);
    exprTest("1<0", ExpressionValue(false), false);
    exprTest("0>1", ExpressionValue(false), false);
    exprTest("1.0==0.0", ExpressionValue(false), false);
    exprTest("bee==wasp", ExpressionValue(false), false); // TODO: figure out why this test fails.
}

} // namespace UnitTest

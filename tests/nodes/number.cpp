// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/number.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(NumberTest, Construction)
{
    Number a;

    // Type should default to integer, value to zero
    EXPECT_TRUE(a.isInteger());
    EXPECT_FALSE(a.isDouble());
    EXPECT_EQ(a.asInteger(), 0);

    // Construct from integer
    Number b(10);
    EXPECT_TRUE(b.isInteger());
    EXPECT_EQ(b.asInteger(), 10);

    // Construct from double
    Number c(1.234);
    EXPECT_TRUE(c.isDouble());
    EXPECT_DOUBLE_EQ(c.asDouble(), 1.234);
}

TEST(NumberTest, BoundedConstruction)
{
    // Construct bounded number from integer
    Number a(10, 5, 20);
    EXPECT_TRUE(a.isBounded());
    EXPECT_TRUE(a.isInteger());
    EXPECT_EQ(a.asInteger(), 10);
    EXPECT_EQ(std::get<int>(a.min().value()), 5);
    EXPECT_EQ(std::get<int>(a.max().value()), 20);

    // Construct bounded number from double
    Number b(1.234, 1.0, 2.0);
    EXPECT_TRUE(b.isBounded());
    EXPECT_TRUE(b.isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 1.234);
    EXPECT_DOUBLE_EQ(std::get<double>(b.min().value()), 1.0);
    EXPECT_DOUBLE_EQ(std::get<double>(b.max().value()), 2.0);
    // Construct bounded number with undefined bounds
    Number c(10);
    EXPECT_FALSE(c.isBounded());

    // Construct bounded number with undefined lower bound
    Number d(10, {}, 20);
    EXPECT_FALSE(c.hasLowerBound());

    // Construct bounded number with undefined upper bound
    Number e(10, 5);
    EXPECT_FALSE(c.hasUpperBound());
}

TEST(NumberTest, Assignment)
{
    Number a;

    // Assignment from integer
    a = 9;
    EXPECT_TRUE(a.isInteger());
    EXPECT_EQ(a.asInteger(), 9);

    // Assignment from double
    a = 5.0;
    EXPECT_TRUE(a.isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 5.0);

    // Assignment from other Number
    auto b = a;
    EXPECT_TRUE(b.isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 5.0);
}

TEST(NumberTest, BoundedAssignment)
{
    // Construct left and right Numbers
    Number a(5, {}, 7);
    Number b(30);

    // Assignment from other Number with upper bound
    a = b;
    EXPECT_EQ(a.asInteger(), 7);

    // Assignment from other Number with lower bound
    Number c(0.5, 0.2);
    Number d(0.15);
    c = d;
    EXPECT_DOUBLE_EQ(c.asDouble(), 0.2);
}

TEST(NumberTest, BLessThanA)
{
    // Int less than int
    Number a(5), b(2);
    EXPECT_TRUE(b < a);

    // Double less than int
    b = 2.0;
    EXPECT_TRUE(b < a);

    // Double less than double
    a = 5.0;
    EXPECT_TRUE(b < a);

    // Unbounded int less than upper bounded int
    Number c(5, {}, 10), d(2);
    EXPECT_TRUE(d < c);

    // Unbounded double less than upper bounded int
    d = 2.0;
    EXPECT_TRUE(d < c);

    // Unbounded double less than lower bounded int
    Number e(5, 2), f(2);
    EXPECT_TRUE(f < e);

    // Unbounded double less than lower bounded int
    f = 2.0;
    EXPECT_TRUE(f < e);

    // Bounded double less than bounded int
    Number g(5, 2, 10), h(2.0, 1.0, 20.0);
    EXPECT_FALSE(g < h);
}

TEST(NumberTest, AGreaterThanB)
{
    // Int greater than int
    Number a(5), b(2);
    EXPECT_TRUE(a > b);

    // Int greater than double
    b = 2.0;
    EXPECT_TRUE(a > b);

    // Double greater than double
    a = 5.0;
    EXPECT_TRUE(a > b);

    // Upper bounded int greater than unbounded int
    Number c(5, {}, 10), d(2);
    EXPECT_TRUE(c > d);

    // Upper bounded int greater than unbounded double
    d = 2.0;
    EXPECT_TRUE(c > d);

    // Lower bounded int greater than unbounded double
    Number e(5, 2), f(2);
    EXPECT_TRUE(e > f);

    // Lower bounded int greater than unbounded double
    f = 2.0;
    EXPECT_TRUE(e > f);

    // Bounded int greater than bounded double
    Number g(5, 2, 10), h(2.0, 1.0, 20.0);
    EXPECT_FALSE(h > g);
}

TEST(NumberTest, Addition)
{
    // Addition of two integers must result in an integer
    Number a(5), b(10);
    EXPECT_TRUE((a + b).isInteger());
    EXPECT_EQ((a + b).asInteger(), a.asInteger() + b.asInteger());

    // Addition of any other combination results in a double
    b = 10.0;
    EXPECT_TRUE((a + b).isDouble());
    EXPECT_DOUBLE_EQ((a + b).asDouble(), a.asDouble() + b.asDouble());
    a = 5.0;
    EXPECT_TRUE((a + b).isDouble());
    EXPECT_DOUBLE_EQ((a + b).asDouble(), a.asDouble() + b.asDouble());
    b = 10;
    EXPECT_TRUE((a + b).isDouble());
    EXPECT_DOUBLE_EQ((a + b).asDouble(), a.asDouble() + b.asDouble());
}

TEST(NumberTest, BoundedAddition)
{
    // Addition of two integers must not exceed the upper bound
    Number a(5, {}, 10), b(10);
    EXPECT_EQ((a + b).asInteger(), 15);

    // Addition of any other combination results in a double
    b = 10.0;
    EXPECT_DOUBLE_EQ((a + b).asDouble(), 15.0);
}

TEST(NumberTest, AdditionAssignment)
{
    // Addition of two integers always results in an integer
    Number a(1);
    EXPECT_TRUE((a += 2).isInteger());
    EXPECT_EQ(a.asInteger(), 3);

    // Addition of a double to an integer results in a double
    EXPECT_TRUE((a += 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 5.0);

    // Addition to a double always results in a double
    a = 1.0;
    EXPECT_TRUE((a += 2).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 3.0);
    EXPECT_TRUE((a += 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 5.0);
}

TEST(NumberTest, BoundedAdditionAssignment)
{
    // Addition of anything to an integer maintains the integer type
    Number a(1, {}, 2);
    EXPECT_TRUE((a += 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a += 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 2);

    // Addition to a double always results in a double
    Number b(1.0, {}, 2.0);
    EXPECT_TRUE((b += 2).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 2.0);
    EXPECT_TRUE((b += 2.0).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 2.0);
}

TEST(NumberTest, Subtraction)
{
    // Subtraction of two integers must result in an integer
    Number a(5), b(10);
    EXPECT_TRUE((a - b).isInteger());
    EXPECT_EQ((a - b).asInteger(), a.asInteger() - b.asInteger());

    // Subtraction of any other combination results in a double
    b = 10.0;
    EXPECT_TRUE((a - b).isDouble());
    EXPECT_DOUBLE_EQ((a - b).asDouble(), a.asDouble() - b.asDouble());
    a = 5.0;
    EXPECT_TRUE((a - b).isDouble());
    EXPECT_DOUBLE_EQ((a - b).asDouble(), a.asDouble() - b.asDouble());
    b = 10;
    EXPECT_TRUE((a - b).isDouble());
    EXPECT_DOUBLE_EQ((a - b).asDouble(), a.asDouble() - b.asDouble());
}

TEST(NumberTest, BoundedSubtraction)
{
    // Subtraction of two integers must not exceed the upper bound
    Number a(5, 2), b(10);
    EXPECT_EQ((a - b).asInteger(), -5);

    // Subtraction of any other combination results in a double
    b = 10.0;
    EXPECT_DOUBLE_EQ((a - b).asDouble(), -5.0);
}

TEST(NumberTest, SubtractionAssignment)
{
    // Subtraction of an integer from an integer maintains the integer type
    Number a(1);
    EXPECT_TRUE((a -= 2).isInteger());
    EXPECT_EQ(a.asInteger(), -1);

    // Subtraction of a double from an integer results in a double
    EXPECT_TRUE((a -= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), -3.0);

    // Subtraction from a double always results in a double
    a = 1.0;
    EXPECT_TRUE((a -= 2).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), -1.0);
    EXPECT_TRUE((a -= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), -3.0);
}

TEST(NumberTest, BoundedSubtractionAssignment)
{
    // Subtraction of anything to an integer maintains the integer type
    Number a(1, 0);
    EXPECT_TRUE(a.min());
    EXPECT_TRUE((a -= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 0);
    EXPECT_TRUE((a -= 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 0);

    // Subtraction to a double always results in a double
    Number b(1.0, 0.5);
    EXPECT_TRUE((b -= 2).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 0.5);
    EXPECT_TRUE((b -= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 0.5);
}

TEST(NumberTest, Multiply)
{
    // Multiplication of two integers must result in an integer
    Number a(5), b(10);
    EXPECT_TRUE((a * b).isInteger());
    EXPECT_EQ((a * b).asInteger(), a.asInteger() * b.asInteger());

    // Multiplication of any other combination results in a double
    b = 10.0;

    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), a.asDouble() * b.asDouble());
    a = 5.0;

    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), a.asDouble() * b.asDouble());
    b = 10;

    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), a.asDouble() * b.asDouble());
}

TEST(NumberTest, BoundedMultiply)
{
    // Multiplication of two integers must result in an integer
    Number a(5, {}, 10), b(10);
    EXPECT_TRUE((a * b).isInteger());
    EXPECT_EQ((a * b).asInteger(), 50);

    // Multiplication of any other combination results in a double
    b = 10.0;
    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), 50);

    a = 5.0;
    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), 50);

    b = 10;
    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), 50);
}

TEST(NumberTest, MultiplyAssignment)
{
    // Multiplication of integer with an integer maintains the integer type
    Number a(1);
    EXPECT_TRUE((a *= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);

    // Multiplying an integer with a double results in a double
    EXPECT_TRUE((a *= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 4.0);

    // Multiplication of a double always results in a double
    a = 1.0;
    EXPECT_TRUE((a *= 2).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 2.0);
    EXPECT_TRUE((a *= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 4.0);
}

TEST(NumberTest, BoundedMultiplyAssignment)
{
    // Multiplication of anything with an integer maintains the integer type
    Number a(1, {}, 2);
    EXPECT_FALSE(a.hasLowerBound());
    EXPECT_TRUE((a *= 4).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a *= 4.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 2);

    // Multiplication of a double always results in a double
    Number b(1.0, {}, 2.0);
    EXPECT_TRUE((b *= 2).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 2.0);
    EXPECT_TRUE((b *= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 2.0);
}

TEST(NumberTest, Division)
{
    // Division of two integers must result in an integer
    Number a(5), b(10);
    EXPECT_TRUE((a / b).isInteger());
    EXPECT_EQ((a / b).asInteger(), a.asInteger() / b.asInteger());

    // Addition of any other combination results in a double
    b = 10.0;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), a.asDouble() / b.asDouble());
    a = 5.0;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), a.asDouble() / b.asDouble());
    b = 10;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), a.asDouble() / b.asDouble());
}

TEST(NumberTest, BoundedDivision)
{
    // Division of two integers must result in an integer
    Number a(5, 2), b(10);
    EXPECT_TRUE((a / b).isInteger());
    EXPECT_EQ((a / b).asInteger(), a.asInteger() / b.asInteger());

    // Addition of any other combination results in a double
    b = 10.0;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), a.asDouble() / b.asDouble());
    a = 5.0;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), a.asDouble() / b.asDouble());
    b = 10;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), a.asDouble() / b.asDouble());
}

TEST(NumberTest, DivisionAssignment)
{
    // Division of an integer by an integer maintains the integer type
    Number a(2);
    EXPECT_TRUE((a /= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 1);

    // Dividing an integer by a double results in a double
    EXPECT_TRUE((a /= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 0.5);

    // Division of a double always results in a double
    a = 2.0;
    EXPECT_TRUE((a /= 2).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 1.0);
    EXPECT_TRUE((a /= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(a.asDouble(), 0.5);
}

TEST(NumberTest, BoundedDivisionAssignment)
{
    // Division of an integer by anything maintains the integer type
    Number a(2, 2);
    EXPECT_TRUE((a /= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a /= 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 2);

    // Division of a double always results in a double
    Number b(2.0, 2.0);
    EXPECT_TRUE((b /= 2).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 2.0);
    EXPECT_TRUE((b /= 2.0).isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 2.0);
}
} // namespace UnitTest

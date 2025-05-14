// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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
    EXPECT_EQ(a.integerMin().value(), 5);
    EXPECT_EQ(a.integerMax().value(), 20);

    // Construct bounded number from double
    Number b(1.234, 1.0, 2.0);
    EXPECT_TRUE(b.isBounded());
    EXPECT_TRUE(b.isDouble());
    EXPECT_DOUBLE_EQ(b.asDouble(), 1.234);
    EXPECT_DOUBLE_EQ(b.doubleMin().value(), 1.0);
    EXPECT_DOUBLE_EQ(b.doubleMax().value(), 2.0);

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
    // Assignment from other Number with upper bound
    Number a(5, {}, 7);
    Number b(30);
    a = b;
    EXPECT_TRUE(a.isInteger());
    EXPECT_TRUE(a.hasUpperBound());
    EXPECT_FALSE(a.hasLowerBound());
    EXPECT_FALSE(a.integerMin());
    EXPECT_EQ(a.asInteger(), 7);

    // Assignment from other Number with lower bound
    Number c(0.5, 0.2);
    Number d(0.15);
    c = d;
    EXPECT_TRUE(c.isDouble());
    EXPECT_TRUE(c.hasLowerBound());
    EXPECT_FALSE(c.hasUpperBound());
    EXPECT_DOUBLE_EQ(c.doubleMax(), 0.2);
    EXPECT_DOUBLE_EQ(c.asDouble(), 0.2);
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
    EXPECT_FALSE(a.integerMin());
    EXPECT_EQ((a + b).asInteger(), 10);

    // Addition of any other combination results in a double
    b = 10.0;
    EXPECT_DOUBLE_EQ((a + b).asDouble(), 10.0);
}

TEST(NumberTest, AdditionAssignment)
{
    // Addition of anything to an integer maintains the integer type
    Number a(1);
    EXPECT_TRUE((a += 2).isInteger());
    EXPECT_EQ(a.asInteger(), 3);
    EXPECT_TRUE((a += 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 5);

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
    EXPECT_FALSE(a.integerMin());
    EXPECT_TRUE((a += 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a += 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 2);

    // Addition to a double always results in a double
    Number b(1.0, {}, 2.0);
    EXPECT_FALSE(a.doubleMin());
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
    EXPECT_FALSE(a.integerMax());
    EXPECT_EQ((a - b).asInteger(), 2);

    // Subtraction of any other combination results in a double
    b = 10.0;
    EXPECT_DOUBLE_EQ((a - b).asDouble(), 2.0);
}

TEST(NumberTest, SubtractionAssignment)
{
    // Subtraction of anything from an integer maintains the integer type
    Number a(1);
    EXPECT_TRUE((a -= 2).isInteger());
    EXPECT_EQ(a.asInteger(), -1);
    EXPECT_TRUE((a -= 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), -3);

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
    EXPECT_FALSE(a.integerMin());
    EXPECT_TRUE((a -= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a -= 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 0);

    // Subtraction to a double always results in a double
    Number b(1.0, 0.5);
    EXPECT_FALSE(a.doubleMin());
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
    EXPECT_FALSE(a.hasLowerBound());
    EXPECT_FALSE(b.isBounded());
    EXPECT_TRUE((a * b).isInteger());
    EXPECT_EQ((a * b).asInteger(), 10);

    // Multiplication of any other combination results in a double
    b = 10.0;

    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), 10);
    a = 5.0;

    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), 10);
    b = 10;

    EXPECT_TRUE((a * b).isDouble());
    EXPECT_DOUBLE_EQ((a * b).asDouble(), 10);
}

TEST(NumberTest, MultiplyAssignment)
{
    // Multiplication of anything with an integer maintains the integer type
    Number a(1);
    EXPECT_TRUE((a *= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a *= 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 4);

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
    EXPECT_TRUE((a *= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 2);
    EXPECT_TRUE((a *= 2.0).isInteger());
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
    EXPECT_EQ((a / b).asInteger(), 2);

    // Addition of any other combination results in a double
    b = 10.0;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), 2.0);
    a = 5.0;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), 2.0);
    b = 10;
    EXPECT_TRUE((a / b).isDouble());
    EXPECT_DOUBLE_EQ((a / b).asDouble(), 2.0);
}

TEST(NumberTest, DivisionAssignment)
{
    // Division of an integer by anything maintains the integer type
    Number a(2);
    EXPECT_TRUE((a /= 2).isInteger());
    EXPECT_EQ(a.asInteger(), 1);
    EXPECT_TRUE((a /= 2.0).isInteger());
    EXPECT_DOUBLE_EQ(a.asInteger(), 0);

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

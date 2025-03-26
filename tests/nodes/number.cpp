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

} // namespace UnitTest

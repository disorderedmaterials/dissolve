// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/potentialSet.h"
#include "math/data1D.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(PotentialSetTest, SimpleAddition)
{
    PotentialSet pots;
    Data1D x;
    const auto value = 2.0;
    x.addPoint(1, value);
    pots.potentialMap()["A-A"].potential = x;
    pots.potentialMap()["A-B"].potential = x;
    pots.potentialMap()["A-C"].potential = x;

    pots += pots;
    EXPECT_EQ(4.0, pots.potentialMap()["A-A"].potential.value(0));
    EXPECT_EQ(4.0, pots.potentialMap()["A-B"].potential.value(0));
    EXPECT_EQ(4.0, pots.potentialMap()["A-C"].potential.value(0));
}

TEST(PotentialSetTest, Multiplication)
{
    PotentialSet pots;
    Data1D x;
    const auto value = 3.0;
    x.addPoint(1, value);
    pots.potentialMap()["A-A"].potential = x;
    pots.potentialMap()["A-B"].potential = x;
    pots.potentialMap()["A-C"].potential = x;

    pots *= 2;
    EXPECT_EQ(6.0, pots.potentialMap()["A-A"].potential.value(0));
    EXPECT_EQ(6.0, pots.potentialMap()["A-B"].potential.value(0));
    EXPECT_EQ(6.0, pots.potentialMap()["A-C"].potential.value(0));
}

TEST(PotentialSetTest, ComplexAddition)
{
    PotentialSet pots;
    PotentialSet pots2;
    Data1D x;
    const auto value = 2.0;
    x.addPoint(1, value);
    pots.potentialMap()["A-A"].potential = x;
    pots.potentialMap()["A-B"].potential = x;
    pots.potentialMap()["A-C"].potential = x;

    pots2.potentialMap()["A-A"].potential = x;
    pots2.potentialMap()["A-B"].potential = x;
    pots2.potentialMap()["A-C"].potential = x;
    pots2.potentialMap()["A-D"].potential = x;

    pots += pots2;
    EXPECT_EQ(4.0, pots.potentialMap()["A-A"].potential.value(0));
    EXPECT_EQ(4.0, pots.potentialMap()["A-B"].potential.value(0));
    EXPECT_EQ(4.0, pots.potentialMap()["A-C"].potential.value(0));
    EXPECT_EQ(2.0, pots.potentialMap()["A-D"].potential.value(0));
}

} // namespace UnitTest

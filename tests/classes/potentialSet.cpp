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
    pots.potentialMap()["A-A"].ep = x;
    pots.potentialMap()["A-B"].ep = x;
    pots.potentialMap()["A-C"].ep = x;

    pots += pots;
    EXPECT_EQ(4, pots.potentialMap()["A-A"].ep.value(0));
    EXPECT_EQ(4, pots.potentialMap()["A-B"].ep.value(0));
    EXPECT_EQ(4, pots.potentialMap()["A-C"].ep.value(0));
}

TEST(PotentialSetTest, Multiplication)
{
    PotentialSet pots;
    Data1D x;
    const auto value = 3.0;
    x.addPoint(1, value);
    pots.potentialMap()["A-A"].ep = x;
    pots.potentialMap()["A-B"].ep = x;
    pots.potentialMap()["A-C"].ep = x;

    pots *= 2;
    EXPECT_EQ(6, pots.potentialMap()["A-A"].ep.value(0));
    EXPECT_EQ(6, pots.potentialMap()["A-B"].ep.value(0));
    EXPECT_EQ(6, pots.potentialMap()["A-C"].ep.value(0));
}

} // namespace UnitTest

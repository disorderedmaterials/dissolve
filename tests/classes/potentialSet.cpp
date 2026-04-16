// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/potentialSet.h"
#include "classes/atomType.h"
#include "math/data1D.h"
#include "math/history.h"
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
    pots.potentials()["A-A"] = x;
    pots.potentials()["A-B"] = x;
    pots.potentials()["A-C"] = x;

    pots += pots;
    EXPECT_EQ(4.0, pots.potentials()["A-A"].value(0));
    EXPECT_EQ(4.0, pots.potentials()["A-B"].value(0));
    EXPECT_EQ(4.0, pots.potentials()["A-C"].value(0));
}

TEST(PotentialSetTest, Multiplication)
{
    PotentialSet pots;
    Data1D x;
    const auto value = 3.0;
    x.addPoint(1, value);
    pots.potentials()["A-A"] = x;
    pots.potentials()["A-B"] = x;
    pots.potentials()["A-C"] = x;

    pots *= 2;
    EXPECT_EQ(6.0, pots.potentials()["A-A"].value(0));
    EXPECT_EQ(6.0, pots.potentials()["A-B"].value(0));
    EXPECT_EQ(6.0, pots.potentials()["A-C"].value(0));
}

TEST(PotentialSetTest, ComplexAddition)
{
    PotentialSet pots;
    PotentialSet pots2;
    Data1D x;
    const auto value = 2.0;
    x.addPoint(1, value);
    pots.potentials()["A-A"] = x;
    pots.potentials()["A-B"] = x;
    pots.potentials()["A-C"] = x;

    pots2.potentials()["A-A"] = x;
    pots2.potentials()["A-B"] = x;
    pots2.potentials()["A-C"] = x;
    pots2.potentials()["A-D"] = x;

    pots += pots2;
    EXPECT_EQ(4.0, pots.potentials()["A-A"].value(0));
    EXPECT_EQ(4.0, pots.potentials()["A-B"].value(0));
    EXPECT_EQ(4.0, pots.potentials()["A-C"].value(0));
    EXPECT_EQ(2.0, pots.potentials()["A-D"].value(0));
}

TEST(PotentialSetTest, Averaging)
{
    Data1D x;
    Data1D y;
    const auto value = 2.0;
    const auto value2 = 4.0;
    const auto averagingLength = 10;

    x.addPoint(1, value);
    y.addPoint(1, value2);

    History<PotentialSet> history;
    for (auto n = 0; n <= 2 * averagingLength; n++)
    {
        PotentialSet pots;

        pots.potentials()["A-A"] = x;
        pots.potentials()["A-B"] = x;
        pots.potentials()["A-C"] = y;
        pots.potentials()["A-D"] = y;

        auto averagedPots = history.push(pots, averagingLength);

        EXPECT_EQ(2.0, averagedPots.potentials()["A-A"].value(0));
        EXPECT_EQ(2.0, averagedPots.potentials()["A-A"].value(0));
        EXPECT_EQ(2.0, averagedPots.potentials()["A-B"].value(0));
        EXPECT_EQ(4.0, averagedPots.potentials()["A-C"].value(0));
        EXPECT_EQ(4.0, averagedPots.potentials()["A-D"].value(0));
    }

    // Round-trip the data via TOML
    SerialisedValue toml;
    history.serialise("history", toml);
    history.clear();

    history.deserialise(toml["history"]);

    auto averagedPots = history.average();

    EXPECT_EQ(2.0, averagedPots.potentials()["A-A"].value(0));
    EXPECT_EQ(2.0, averagedPots.potentials()["A-A"].value(0));
    EXPECT_EQ(2.0, averagedPots.potentials()["A-B"].value(0));
    EXPECT_EQ(4.0, averagedPots.potentials()["A-C"].value(0));
    EXPECT_EQ(4.0, averagedPots.potentials()["A-D"].value(0));
}

} // namespace UnitTest

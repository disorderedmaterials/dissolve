// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/history.h"
#include "nodes/number.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(History, SimpleIntegerNumbers)
{
    History<Number> i;
    const auto value = 12345;
    for (auto n = 0; n < 10; ++n)
        EXPECT_EQ(i.push(value, 5).asInteger(), value);
}

TEST(PODHistory, SimpleIntegerNumbers)
{
    PODHistory<int> i;
    const auto value = 12345;
    for (auto n = 0; n < 10; ++n)
        EXPECT_EQ(i.pushAndAverage(value, 5), value);
}

TEST(History, SimpleDoubleNumbers)
{
    History<Number> d;
    const auto avgLength = 3;
    EXPECT_DOUBLE_EQ(d.push(1.0, avgLength).asDouble(), 1.0);
    EXPECT_DOUBLE_EQ(d.push(2.0, avgLength).asDouble(), (1 + 2) / 2.0);
    EXPECT_DOUBLE_EQ(d.push(3.0, avgLength).asDouble(), (1 + 2 + 3) / 3.0);
    EXPECT_DOUBLE_EQ(d.push(4.0, avgLength).asDouble(), (2 + 3 + 4) / 3.0);
    EXPECT_DOUBLE_EQ(d.push(5.0, avgLength).asDouble(), (3 + 4 + 5) / 3.0);
}

TEST(PODHistory, SimpleDoubleNumbers)
{
    PODHistory<double> d;
    const auto avgLength = 3;
    EXPECT_DOUBLE_EQ(d.pushAndAverage(1.0, avgLength), 1.0);
    EXPECT_DOUBLE_EQ(d.pushAndAverage(2.0, avgLength), (1 + 2) / 2.0);
    EXPECT_DOUBLE_EQ(d.pushAndAverage(3.0, avgLength), (1 + 2 + 3) / 3.0);
    EXPECT_DOUBLE_EQ(d.pushAndAverage(4.0, avgLength), (2 + 3 + 4) / 3.0);
    EXPECT_DOUBLE_EQ(d.pushAndAverage(5.0, avgLength), (3 + 4 + 5) / 3.0);
}

TEST(History, SimpleDeserialisation)
{
    History<Number> a, b;

    const auto avgLength = 5;
    a.push(1.0, avgLength).asDouble();
    a.push(2.0, avgLength).asDouble();
    a.push(3.0, avgLength).asDouble();
    a.push(4.0, avgLength).asDouble();
    a.push(5.0, avgLength).asDouble();

    tomlRoundTrip(a, b);
    EXPECT_EQ(a.average(), b.average());

    // Push another value to b to test clearing.
    b.push(100.0, avgLength);
    tomlRoundTrip(a, b);
    EXPECT_EQ(a.average(), b.average());
}

TEST(History, CustomClass)
{
    History<Data1D> a, b;
    const auto avgLength = 5;
    Data1D d;
    for (auto x = 0; x < 100; ++x)
        d.addPoint(x * 0.1, cos(x * 0.1), sin(x * 0.1));

    auto sum = 0.0;
    for (auto n = 1; n <= 5; ++n)
    {
        sum += n;
        EXPECT_TRUE(DissolveSystemTest::checkData1D(d * (sum / n), "Original", a.push(d * n, avgLength), "Averaged"));
    }

    tomlRoundTrip(a, b);
    EXPECT_TRUE(DissolveSystemTest::checkData1D(a.average(), "A", b.average(), "B"));
}

TEST(History, CustomClassWithInitialiser)
{
    const auto avgLength = 3;
    auto Arnode = TestGraph::createAtomicSpecies(Elements::Ar);
    auto &Ar = Arnode->species();

    KeyedVector<const Species *, int> pop;
    pop.add(&Ar, 100);

    History<PartialSet> a(
        [&]()
        {
            PartialSet p;
            p.initialise(pop);
            return p;
        }),
        b(
            [&]()
            {
                PartialSet p;
                p.initialise(pop);
                return p;
            });

    Data1D dcos, dsin;
    for (auto x = 0; x < 100; ++x)
    {
        dcos.addPoint(x * 0.1, cos(x * 0.1));
        dsin.addPoint(x * 0.1, sin(x * 0.1));
    }

    PartialSet p;
    p.initialise(pop);
    p.partials().get("Ar//Ar") = dcos + dsin;
    p.boundPartials().get("Ar//Ar") = dcos;
    p.unboundPartials().get("Ar//Ar") = dsin;
    p.boundTotal() = dcos;
    p.unboundTotal() = dsin;
    p.total() = dcos * 2.0;

    // Accumulate same data
    for (auto n = 0; n < 3; ++n)
    {
        auto avg = a.push(p, avgLength);
        EXPECT_TRUE(
            DissolveSystemTest::checkData1D(p.partials().get("Ar//Ar"), "Partial", avg.partials().get("Ar//Ar"), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D(p.boundPartials().get("Ar//Ar"), "BoundPartial",
                                                    avg.boundPartials().get("Ar//Ar"), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D(p.boundTotal(), "BoundTotal", avg.boundTotal(), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D(p.unboundPartials().get("Ar//Ar"), "UnboundPartial",
                                                    avg.unboundPartials().get("Ar//Ar"), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D(p.unboundTotal(), "UnboundTotal", avg.unboundTotal(), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D(p.total(), "Total", avg.total(), "Averaged"));
    }

    // Accumulate opposite trig values - just test partials as the totals are not automatically modified by PartialSet
    p.boundPartials().get("Ar//Ar") = dsin * -1.0;
    p.unboundPartials().get("Ar//Ar") = dcos * -1.0;
    auto sum = 0.0;
    for (auto n = 1; n <= 3; ++n)
    {
        auto avg = a.push(p, avgLength);
        EXPECT_TRUE(
            DissolveSystemTest::checkData1D(p.partials().get("Ar//Ar"), "Partial", avg.partials().get("Ar//Ar"), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D((dcos * (avgLength - n) - dsin * n) / avgLength, "BoundPartial",
                                                    avg.boundPartials().get("Ar//Ar"), "Averaged"));
        EXPECT_TRUE(DissolveSystemTest::checkData1D((dsin * (avgLength - n) - dcos * n) / avgLength, "UnboundPartial",
                                                    avg.unboundPartials().get("Ar//Ar"), "Averaged"));
    }

    tomlRoundTrip(a, b);
    auto avgA = a.average();
    auto avgB = b.average();
    EXPECT_TRUE(DissolveSystemTest::checkData1D(avgA.boundPartials().get("Ar//Ar"), "BoundPartialA",
                                                avgB.boundPartials().get("Ar//Ar"), "BoundPartialB"));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(avgA.unboundPartials().get("Ar//Ar"), "UnboundPartialA",
                                                avgB.unboundPartials().get("Ar//Ar"), "UnboundPartialB"));
}

} // namespace UnitTest
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/potentialSet.h"
#include "classes/atomType.h"
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

TEST(PotentialSetTest, Averaging)
{
    GenericList moduleData;
    auto originalPotentialsObject =
        moduleData.realiseIf<PotentialSet>("PotentialSet", "module", GenericItem::InRestartFileFlag);

    std::string filename{std::format("{}.restart.txt", ::testing::UnitTest::GetInstance()->current_test_info()->name())};
    // Open the file
    LineParser parser;
    ASSERT_TRUE(parser.openOutput(filename));

    Data1D x;
    Data1D y;
    const auto value = 2.0;
    const auto value2 = 4.0;
    const auto averagingLength = 10;

    auto A = std::make_shared<AtomType>();
    auto B = std::make_shared<AtomType>();
    auto C = std::make_shared<AtomType>();
    auto D = std::make_shared<AtomType>();

    A->setName("A");
    B->setName("B");
    C->setName("C");
    D->setName("D");

    x.addPoint(1, value);
    y.addPoint(1, value2);

    for (auto n = 0; n <= 2 * averagingLength; n++)
    {
        PotentialSet pots;
        pots.potentialMap()["A-A"].at1 = A;
        pots.potentialMap()["A-A"].at2 = A;
        pots.potentialMap()["A-A"].potential = x;

        pots.potentialMap()["A-B"].at1 = A;
        pots.potentialMap()["A-B"].at2 = B;
        pots.potentialMap()["A-B"].potential = x;

        pots.potentialMap()["A-C"].at1 = A;
        pots.potentialMap()["A-C"].at2 = C;
        pots.potentialMap()["A-C"].potential = y;

        pots.potentialMap()["A-D"].at1 = A;
        pots.potentialMap()["A-D"].at2 = D;
        pots.potentialMap()["A-D"].potential = y;

        originalPotentialsObject.first = pots;
        Averaging::average<PotentialSet>(moduleData, "PotentialSet", "module", averagingLength,
                                         Averaging::AveragingScheme::LinearAveraging);

        EXPECT_EQ(A, pots.potentialMap()["A-A"].at1);
        EXPECT_EQ(A, pots.potentialMap()["A-A"].at2);
        EXPECT_EQ(A, pots.potentialMap()["A-B"].at1);
        EXPECT_EQ(B, pots.potentialMap()["A-B"].at2);
        EXPECT_EQ(A, pots.potentialMap()["A-C"].at1);
        EXPECT_EQ(C, pots.potentialMap()["A-C"].at2);
        EXPECT_EQ(A, pots.potentialMap()["A-D"].at1);
        EXPECT_EQ(D, pots.potentialMap()["A-D"].at2);

        EXPECT_EQ(2.0, pots.potentialMap()["A-A"].potential.value(0));
        EXPECT_EQ(2.0, pots.potentialMap()["A-A"].potential.value(0));
        EXPECT_EQ(2.0, pots.potentialMap()["A-B"].potential.value(0));
        EXPECT_EQ(4.0, pots.potentialMap()["A-C"].potential.value(0));
        EXPECT_EQ(4.0, pots.potentialMap()["A-D"].potential.value(0));

        pots.serialise(parser);
    }
}

TEST(PotentialSetTest, Averaging2)
{
    GenericList moduleData;
    auto originalPotentialsObject =
        moduleData.realiseIf<PotentialSet>("PotentialSet", "module", GenericItem::InRestartFileFlag);

    std::string filename{std::format("{}.restart.txt", ::testing::UnitTest::GetInstance()->current_test_info()->name())};

    // Open the file
    LineParser parser;
    if (!parser.openOutput(filename))
    {
        parser.closeFiles();
    }

    Data1D x;
    Data1D y;
    const auto value = 2.0;
    const auto value2 = 4.0;
    const auto averagingLength = 10;

    auto A = std::make_shared<AtomType>();
    auto B = std::make_shared<AtomType>();
    auto C = std::make_shared<AtomType>();
    auto D = std::make_shared<AtomType>();

    A->setName("A");
    B->setName("B");
    C->setName("C");
    D->setName("D");

    x.addPoint(1, value);
    y.addPoint(1, value2);

    for (auto n = 0; n <= 2 * averagingLength; n++)
    {
        PotentialSet pots;
        pots.potentialMap()["A-A"].at1 = A;
        pots.potentialMap()["A-A"].at2 = A;
        pots.potentialMap()["A-A"].potential = x;

        pots.potentialMap()["A-B"].at1 = A;
        pots.potentialMap()["A-B"].at2 = B;
        pots.potentialMap()["A-B"].potential = x;

        pots.potentialMap()["A-C"].at1 = A;
        pots.potentialMap()["A-C"].at2 = C;
        pots.potentialMap()["A-C"].potential = y;

        pots.potentialMap()["A-D"].at1 = A;
        pots.potentialMap()["A-D"].at2 = D;
        pots.potentialMap()["A-D"].potential = y;

        originalPotentialsObject.first = pots;
        Averaging::average<PotentialSet>(moduleData, "PotentialSet", "module", averagingLength,
                                         Averaging::AveragingScheme::LinearAveraging);

        EXPECT_EQ(A, pots.potentialMap()["A-A"].at1);
        EXPECT_EQ(A, pots.potentialMap()["A-A"].at2);
        EXPECT_EQ(A, pots.potentialMap()["A-B"].at1);
        EXPECT_EQ(B, pots.potentialMap()["A-B"].at2);
        EXPECT_EQ(A, pots.potentialMap()["A-C"].at1);
        EXPECT_EQ(C, pots.potentialMap()["A-C"].at2);
        EXPECT_EQ(A, pots.potentialMap()["A-D"].at1);
        EXPECT_EQ(D, pots.potentialMap()["A-D"].at2);

        EXPECT_EQ(2.0, pots.potentialMap()["A-A"].potential.value(0));
        EXPECT_EQ(2.0, pots.potentialMap()["A-A"].potential.value(0));
        EXPECT_EQ(2.0, pots.potentialMap()["A-B"].potential.value(0));
        EXPECT_EQ(4.0, pots.potentialMap()["A-C"].potential.value(0));
        EXPECT_EQ(4.0, pots.potentialMap()["A-D"].potential.value(0));

        pots.serialise(parser);
    }
}

} // namespace UnitTest

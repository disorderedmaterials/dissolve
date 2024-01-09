// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

TEST(EmpiricalFormulaTest, Order)
{
    EmpiricalFormula::EmpiricalFormulaMap testMap;
    testMap[Elements::H] = 3;
    testMap[Elements::N] = 1;
    testMap[Elements::C] = 2;
    EXPECT_EQ(EmpiricalFormula::formula(testMap), "NC2H3");
}

TEST(EmpiricalFormulaTest, Zeroes)
{
    EmpiricalFormula::EmpiricalFormulaMap testMap;
    testMap[Elements::Na] = 0;
    testMap[Elements::Mg] = 1;
    testMap[Elements::Al] = 0;
    EXPECT_EQ(EmpiricalFormula::formula(testMap), "Mg");
}

TEST(EmpiricalFormulaTest, Examples)
{
    EXPECT_EQ(EmpiricalFormula::formula(UnitTest::benzeneSpecies().atoms(), [](const auto &i) { return i.Z(); }), "C6H6");
    EXPECT_EQ(EmpiricalFormula::formula(UnitTest::methaneSpecies().atoms(), [](const auto &i) { return i.Z(); }), "CH4");
    EXPECT_EQ(EmpiricalFormula::formula(UnitTest::methanolSpecies().atoms(), [](const auto &i) { return i.Z(); }), "OCH4");
}

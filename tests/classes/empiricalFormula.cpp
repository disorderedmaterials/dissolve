// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "nodes/structure/species.h"
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
    SpeciesNode benzene(nullptr);
    benzene.load("species/benzene.toml");
    EXPECT_EQ(EmpiricalFormula::formula(benzene.atoms(), [](const auto &i) { return i.Z(); }), "C6H6");

    SpeciesNode methane(nullptr);
    methane.load("species/methane.toml");
    EXPECT_EQ(EmpiricalFormula::formula(methane.atoms(), [](const auto &i) { return i.Z(); }), "CH4");

    SpeciesNode methanol(nullptr);
    methanol.load("species/methanol.toml");
    EXPECT_EQ(EmpiricalFormula::formula(methanol.atoms(), [](const auto &i) { return i.Z(); }), "OCH4");
}

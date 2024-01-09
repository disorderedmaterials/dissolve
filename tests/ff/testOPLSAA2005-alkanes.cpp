// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class OPLSAA2005AlkanesForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/ff-oplsaa2005-alkanes.txt")); }
};

TEST_F(OPLSAA2005AlkanesForcefieldTest, Heptane)
{
    auto *sp = systemTest.coreData().findSpecies("Heptane");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CT3");
    systemTest.checkSpeciesAtomType(sp, 1, "CT2");
    systemTest.checkSpeciesAtomType(sp, 2, "CT2");
    systemTest.checkSpeciesAtomType(sp, 3, "CT2");
    systemTest.checkSpeciesAtomType(sp, 4, "CT3");
    systemTest.checkSpeciesAtomType(sp, 5, "HC");
    systemTest.checkSpeciesAtomType(sp, 6, "HC");
    systemTest.checkSpeciesAtomType(sp, 7, "HC");
    systemTest.checkSpeciesAtomType(sp, 8, "HC");
    systemTest.checkSpeciesAtomType(sp, 9, "HC");
    systemTest.checkSpeciesAtomType(sp, 10, "HC");
    systemTest.checkSpeciesAtomType(sp, 11, "HC");
    systemTest.checkSpeciesAtomType(sp, 12, "HC");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "HC");
    systemTest.checkSpeciesAtomType(sp, 16, "HC");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=2242.62 eq=1.529"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 7}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 9}, {AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=488.273 eq=112.7"});
    systemTest.checkSpeciesIntramolecular(sp, {15, 4, 16}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    systemTest.checkSpeciesIntramolecular(sp, {7, 0, 1, 9}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(sp, {7, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2, 3}, {TorsionFunctions::Form::Cos3, "5.4392  -0.2092  0.8368"});
}

TEST_F(OPLSAA2005AlkanesForcefieldTest, Cycloheptane)
{
    auto *sp = systemTest.coreData().findSpecies("Cycloheptane");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CT2");
    systemTest.checkSpeciesAtomType(sp, 1, "CT2");
    systemTest.checkSpeciesAtomType(sp, 2, "CT2");
    systemTest.checkSpeciesAtomType(sp, 3, "CT2");
    systemTest.checkSpeciesAtomType(sp, 4, "CT2");
    systemTest.checkSpeciesAtomType(sp, 5, "CT2");
    systemTest.checkSpeciesAtomType(sp, 6, "CT2");
    systemTest.checkSpeciesAtomType(sp, 7, "HC");
    systemTest.checkSpeciesAtomType(sp, 8, "HC");
    systemTest.checkSpeciesAtomType(sp, 9, "HC");
    systemTest.checkSpeciesAtomType(sp, 10, "HC");
    systemTest.checkSpeciesAtomType(sp, 11, "HC");
    systemTest.checkSpeciesAtomType(sp, 12, "HC");
    systemTest.checkSpeciesAtomType(sp, 13, "HC");
    systemTest.checkSpeciesAtomType(sp, 14, "HC");
    systemTest.checkSpeciesAtomType(sp, 15, "HC");
    systemTest.checkSpeciesAtomType(sp, 16, "HC");
    systemTest.checkSpeciesAtomType(sp, 17, "HC");
    systemTest.checkSpeciesAtomType(sp, 18, "HC");
    systemTest.checkSpeciesAtomType(sp, 19, "HC");
    systemTest.checkSpeciesAtomType(sp, 20, "HC");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=2242.62 eq=1.529"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 7}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 9}, {AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=488.273 eq=112.7"});
    systemTest.checkSpeciesIntramolecular(sp, {15, 4, 16}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    systemTest.checkSpeciesIntramolecular(sp, {7, 0, 1, 9}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(sp, {7, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2, 3}, {TorsionFunctions::Form::Cos3, "5.4392  -0.2092  0.8368"});
}
}; // namespace UnitTest

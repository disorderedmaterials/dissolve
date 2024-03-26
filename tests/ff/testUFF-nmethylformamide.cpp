// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class UFFNMethylFormamideForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/ff-uff-nmethylformamide.txt")); }
};

TEST_F(UFFNMethylFormamideForcefieldTest, NMethylFormamide)
{
    auto *sp = systemTest.coreData().findSpecies("N-MethylFormamide");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "H_");
    systemTest.checkSpeciesAtomType(sp, 1, "C_amR");
    systemTest.checkSpeciesAtomType(sp, 2, "N_amR");
    systemTest.checkSpeciesAtomType(sp, 3, "O_2");
    systemTest.checkSpeciesAtomType(sp, 4, "C_3");
    systemTest.checkSpeciesAtomType(sp, 5, "H_");
    systemTest.checkSpeciesAtomType(sp, 6, "H_");
    systemTest.checkSpeciesAtomType(sp, 7, "H_");
    systemTest.checkSpeciesAtomType(sp, 8, "H_");
    systemTest.checkSpeciesIntramolecular(sp, {1, 2}, {BondFunctions::Form::Harmonic, "k=5410.2978 eq=1.3568"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=2991.0611 eq=1.08142"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {2, 4}, {BondFunctions::Form::Harmonic, "k=4432.3881 eq=1.45007"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {1, 2, 4}, {AngleFunctions::Form::Cos2, "882.7151 0.5 0.666667 0.333333"},
                                          5.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {3, 1, 2}, {AngleFunctions::Form::Cos2, "1590.3058 0.5 0.6667 0.3333"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {8, 4, 7}, {AngleFunctions::Form::Cos2, "315.8868 0.343737 0.37492 0.281246"},
                                          6.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2, 4}, {TorsionFunctions::Form::UFFCosine, "101.9308  2  180"}, 2.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2, 3}, {TorsionFunctions::Form::FourierN, "209.2 1 -1 0"});
    systemTest.checkSpeciesIntramolecular(sp, {2, 5, 1, 4}, {TorsionFunctions::Form::FourierN, "25.104 1 -1 0"});
}
}; // namespace UnitTest

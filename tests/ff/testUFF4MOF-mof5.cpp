// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class UFF4MOFMOF5ForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/ff-uff4mof-mof5.txt")); }
};

TEST_F(UFF4MOFMOF5ForcefieldTest, MOF5)
{
    auto *sp = systemTest.coreData().findSpecies("MOF5");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "Zn3f2");
    systemTest.checkSpeciesAtomType(sp, 1, "Zn3f2");
    systemTest.checkSpeciesAtomType(sp, 26, "Zn3f2");
    systemTest.checkSpeciesAtomType(sp, 29, "Zn3f2");
    systemTest.checkSpeciesAtomType(sp, 32, "O_f_3");
    systemTest.checkSpeciesAtomType(sp, 71, "O_m_2");
    systemTest.checkSpeciesAtomType(sp, 112, "O_m_2");
    systemTest.checkSpeciesAtomType(sp, 117, "O_m_2");
    systemTest.checkSpeciesAtomType(sp, 121, "O_m_2");
    systemTest.checkSpeciesAtomType(sp, 133, "O_m_2");
    systemTest.checkSpeciesAtomType(sp, 122, "O_m_2");
    systemTest.checkSpeciesAtomType(sp, 156, "C_R");
    systemTest.checkSpeciesAtomType(sp, 178, "C_R");
    systemTest.checkSpeciesAtomType(sp, 226, "C_R");
    systemTest.checkSpeciesAtomType(sp, 290, "C_R");
    systemTest.checkSpeciesAtomType(sp, 386, "H_");
    systemTest.checkSpeciesIntramolecular(sp, {386, 290}, {BondFunctions::Form::Harmonic, "k=2991.0611 eq=1.08142"}, 1.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {0, 32}, {BondFunctions::Form::Harmonic, "k=1337.872 eq=1.84185"}, 3.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {8, 32}, {BondFunctions::Form::Harmonic, "k=1337.872 eq=1.84185"}, 1.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {71, 156}, {BondFunctions::Form::Harmonic, "k=5048.4039 eq=1.34262"}, 5.0e-6);
    systemTest.checkSpeciesIntramolecular(sp, {178, 122, 26}, {AngleFunctions::Form::Cosine, "53.5872 3 0 -1"}, 3.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {121, 29, 112}, {AngleFunctions::Form::Cosine, "107.58286 3 0 -1"}, 6.0e-6);
    systemTest.checkSpeciesIntramolecular(sp, {178, 122, 26, 117}, {TorsionFunctions::Form::UFFCosine, "96.4708  2  180"},
                                          5.0e-5);
    systemTest.checkSpeciesIntramolecular(sp, {178, 122, 133, 226}, {TorsionFunctions::Form::FourierN, "25.104 1 -1 0"});
}
}; // namespace UnitTest

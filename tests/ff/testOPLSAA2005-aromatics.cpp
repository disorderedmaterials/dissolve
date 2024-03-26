// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class OPLSAA2005AromaticsForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/ff-oplsaa2005-aromatics.txt")); }
};

TEST_F(OPLSAA2005AromaticsForcefieldTest, Benzene)
{
    auto *sp = systemTest.coreData().findSpecies("Benzene");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CA");
    systemTest.checkSpeciesAtomType(sp, 1, "HA");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=3071.06 eq=1.08"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 2}, {BondFunctions::Form::Harmonic, "k=3924.59 eq=1.4"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=292.88 eq=120"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 2, 4}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2, 3}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2, 4}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
    systemTest.checkSpeciesIntramolecular(sp, {10, 0, 2, 4}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
}

TEST_F(OPLSAA2005AromaticsForcefieldTest, Naphthalene)
{
    auto *sp = systemTest.coreData().findSpecies("Naphthalene");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CA");
    systemTest.checkSpeciesAtomType(sp, 1, "CA");
    systemTest.checkSpeciesAtomType(sp, 2, "CNap");
    systemTest.checkSpeciesAtomType(sp, 3, "CA");
    systemTest.checkSpeciesAtomType(sp, 4, "CA");
    systemTest.checkSpeciesAtomType(sp, 5, "CA");
    systemTest.checkSpeciesAtomType(sp, 6, "CA");
    systemTest.checkSpeciesAtomType(sp, 7, "CNap");
    systemTest.checkSpeciesAtomType(sp, 8, "CA");
    systemTest.checkSpeciesAtomType(sp, 9, "CA");
    systemTest.checkSpeciesAtomType(sp, 10, "HA");
    systemTest.checkSpeciesAtomType(sp, 11, "HA");
    systemTest.checkSpeciesAtomType(sp, 12, "HA");
    systemTest.checkSpeciesAtomType(sp, 13, "HA");
    systemTest.checkSpeciesAtomType(sp, 14, "HA");
    systemTest.checkSpeciesAtomType(sp, 15, "HA");
    systemTest.checkSpeciesAtomType(sp, 16, "HA");
    systemTest.checkSpeciesAtomType(sp, 17, "HA");
    systemTest.checkSpeciesIntramolecular(sp, {2, 7}, {BondFunctions::Form::Harmonic, "k=3924.59 eq=1.4"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 2, 7}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 2, 3}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    systemTest.checkSpeciesIntramolecular(sp, {8, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
}
}; // namespace UnitTest

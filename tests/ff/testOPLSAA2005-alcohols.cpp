// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class OPLSAA2005AlcoholsForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/ff-oplsaa2005-alcohols.txt")); }
};

TEST_F(OPLSAA2005AlcoholsForcefieldTest, Methanol)
{
    auto *sp = systemTest.coreData().findSpecies("Methanol");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "CT");
    systemTest.checkSpeciesAtomType(sp, 1, "OH");
    systemTest.checkSpeciesAtomType(sp, 2, "HO");
    systemTest.checkSpeciesAtomType(sp, 3, "HC");
    systemTest.checkSpeciesAtomType(sp, 4, "HC");
    systemTest.checkSpeciesAtomType(sp, 5, "HC");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=2677.76 eq=1.41"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 3}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=460.24 eq=108.5"});
    systemTest.checkSpeciesIntramolecular(sp, {4, 0, 1}, {AngleFunctions::Form::Harmonic, "k=292.88 eq=109.5"});
    systemTest.checkSpeciesIntramolecular(sp, {3, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.47444"});
}
}; // namespace UnitTest

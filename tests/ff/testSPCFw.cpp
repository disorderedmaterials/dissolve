// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class SPCFwForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/ff-spcfw.txt")); }
};

TEST_F(SPCFwForcefieldTest, Water)
{
    auto *sp = systemTest.coreData().findSpecies("Water");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "OW");
    systemTest.checkSpeciesAtomType(sp, 1, "HW");
    systemTest.checkSpeciesAtomType(sp, 2, "HW");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=4431.53 eq=1"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=317.5656 eq=113.24"});
}
}; // namespace UnitTest

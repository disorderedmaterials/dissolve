// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class Kulmala2010ForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/ff-kulmala2010.txt")); }
};

TEST_F(Kulmala2010ForcefieldTest, Hydronium)
{
    auto *sp = systemTest.coreData().findSpecies("Hydronium");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "OW+");
    systemTest.checkSpeciesAtomType(sp, 1, "HW+");
    systemTest.checkSpeciesAtomType(sp, 2, "HW+");
    systemTest.checkSpeciesAtomType(sp, 3, "HW+");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=4030 eq=0.969"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=451.4 eq=113.1"});
}

TEST_F(Kulmala2010ForcefieldTest, Ammonia)
{
    auto *sp = systemTest.coreData().findSpecies("Ammonia");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NH");
    systemTest.checkSpeciesAtomType(sp, 1, "HN");
    systemTest.checkSpeciesAtomType(sp, 2, "HN");
    systemTest.checkSpeciesAtomType(sp, 3, "HN");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=3864 eq=1.003"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=334.01 eq=107.18"});
}

TEST_F(Kulmala2010ForcefieldTest, Ammonium)
{
    auto *sp = systemTest.coreData().findSpecies("Ammonium");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NH+");
    systemTest.checkSpeciesAtomType(sp, 1, "HN+");
    systemTest.checkSpeciesAtomType(sp, 2, "HN+");
    systemTest.checkSpeciesAtomType(sp, 3, "HN+");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=3660.3 eq=1.013"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=518.26 eq=109.47"});
}

TEST_F(Kulmala2010ForcefieldTest, Dimethylammonium)
{
    auto *sp = systemTest.coreData().findSpecies("Dimethylammonium");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "NDM");
    systemTest.checkSpeciesAtomType(sp, 1, "HDM");
    systemTest.checkSpeciesAtomType(sp, 2, "HDM");
    systemTest.checkSpeciesAtomType(sp, 3, "CDM");
    systemTest.checkSpeciesAtomType(sp, 4, "CDM");
    systemTest.checkSpeciesAtomType(sp, 5, "HCD");
    systemTest.checkSpeciesAtomType(sp, 6, "HCD");
    systemTest.checkSpeciesAtomType(sp, 7, "HCD");
    systemTest.checkSpeciesAtomType(sp, 8, "HCD");
    systemTest.checkSpeciesAtomType(sp, 9, "HCD");
    systemTest.checkSpeciesAtomType(sp, 10, "HCD");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=3632 eq=1.01"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 3}, {BondFunctions::Form::Harmonic, "k=3071 eq=1.499"});
    systemTest.checkSpeciesIntramolecular(sp, {3, 8}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(sp, {2, 0, 1}, {AngleFunctions::Form::Harmonic, "k=292.9 eq=105.5"});
    systemTest.checkSpeciesIntramolecular(sp, {3, 0, 1}, {AngleFunctions::Form::Harmonic, "k=418.4 eq=109.2"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 3, 8}, {AngleFunctions::Form::Harmonic, "k=209.2 eq=108.5"});
    systemTest.checkSpeciesIntramolecular(sp, {9, 3, 8}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    systemTest.checkSpeciesIntramolecular(sp, {2, 0, 4, 5}, {TorsionFunctions::Form::Cos3, "0  0  0"});
    systemTest.checkSpeciesIntramolecular(sp, {3, 0, 4, 5}, {TorsionFunctions::Form::Cos3, "0  0  3.3765"});
}

TEST_F(Kulmala2010ForcefieldTest, H2SO4)
{
    auto *sp = systemTest.coreData().findSpecies("H2SO4");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "SA");
    systemTest.checkSpeciesAtomType(sp, 1, "OA");
    systemTest.checkSpeciesAtomType(sp, 2, "OA");
    systemTest.checkSpeciesAtomType(sp, 3, "OHA");
    systemTest.checkSpeciesAtomType(sp, 4, "OHA");
    systemTest.checkSpeciesAtomType(sp, 5, "HSA");
    systemTest.checkSpeciesAtomType(sp, 6, "HSA");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=6258 eq=1.411"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 3}, {BondFunctions::Form::Harmonic, "k=3083.8 eq=1.571"});
    systemTest.checkSpeciesIntramolecular(sp, {3, 6}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=224.37 eq=123.69"});
    systemTest.checkSpeciesIntramolecular(sp, {6, 3, 0}, {AngleFunctions::Form::Harmonic, "k=396.82 eq=110.5"});
    systemTest.checkSpeciesIntramolecular(sp, {6, 3, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

TEST_F(Kulmala2010ForcefieldTest, HSO4Minus)
{
    auto *sp = systemTest.coreData().findSpecies("HSO4-");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "SA-");
    systemTest.checkSpeciesAtomType(sp, 1, "OA-");
    systemTest.checkSpeciesAtomType(sp, 2, "OA-");
    systemTest.checkSpeciesAtomType(sp, 3, "OA-");
    systemTest.checkSpeciesAtomType(sp, 4, "OHA-");
    systemTest.checkSpeciesAtomType(sp, 5, "HSA-");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=5239.9 eq=1.438"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 4}, {BondFunctions::Form::Harmonic, "k=2328.3 eq=1.634"});
    systemTest.checkSpeciesIntramolecular(sp, {4, 5}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=456.97 eq=114.76"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 4}, {AngleFunctions::Form::Harmonic, "k=814.48 eq=103.45"});
    systemTest.checkSpeciesIntramolecular(sp, {5, 4, 0}, {AngleFunctions::Form::Harmonic, "k=398.12 eq=106.39"});
    systemTest.checkSpeciesIntramolecular(sp, {5, 4, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class LudwigPy5ForcefieldTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/ff-ludwig-py5.txt")); }
};

TEST_F(LudwigPy5ForcefieldTest, Py5)
{
    auto *sp = systemTest.coreData().findSpecies("Py5");
    ASSERT_TRUE(sp);
    systemTest.checkSpeciesAtomType(sp, 0, "nc");
    systemTest.checkSpeciesAtomType(sp, 1, "ca_o");
    systemTest.checkSpeciesAtomType(sp, 2, "ca_m");
    systemTest.checkSpeciesAtomType(sp, 3, "ca_p");
    systemTest.checkSpeciesAtomType(sp, 4, "ca_m");
    systemTest.checkSpeciesAtomType(sp, 5, "ca_o");
    systemTest.checkSpeciesAtomType(sp, 6, "ha_o");
    systemTest.checkSpeciesAtomType(sp, 7, "ha_m");
    systemTest.checkSpeciesAtomType(sp, 8, "ha_p");
    systemTest.checkSpeciesAtomType(sp, 9, "ha_m");
    systemTest.checkSpeciesAtomType(sp, 10, "ha_o");
    systemTest.checkSpeciesAtomType(sp, 11, "ct_1");
    systemTest.checkSpeciesAtomType(sp, 12, "ct_2");
    systemTest.checkSpeciesAtomType(sp, 13, "hc_1");
    systemTest.checkSpeciesAtomType(sp, 14, "hc_1");
    systemTest.checkSpeciesAtomType(sp, 15, "ct_3");
    systemTest.checkSpeciesAtomType(sp, 16, "hc_2");
    systemTest.checkSpeciesAtomType(sp, 17, "hc_2");
    systemTest.checkSpeciesAtomType(sp, 18, "ct_4");
    systemTest.checkSpeciesAtomType(sp, 19, "hc_3");
    systemTest.checkSpeciesAtomType(sp, 20, "hc_3");
    systemTest.checkSpeciesAtomType(sp, 21, "cm");
    systemTest.checkSpeciesAtomType(sp, 22, "hc_4");
    systemTest.checkSpeciesAtomType(sp, 23, "hc_4");
    systemTest.checkSpeciesAtomType(sp, 24, "hm");
    systemTest.checkSpeciesAtomType(sp, 25, "hm");
    systemTest.checkSpeciesAtomType(sp, 26, "hm");
    systemTest.checkSpeciesIntramolecular(sp, {0, 1}, {BondFunctions::Form::Harmonic, "k=4042 eq=1.339"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 6}, {BondFunctions::Form::Harmonic, "k=3071 eq=1.08"});
    systemTest.checkSpeciesIntramolecular(sp, {11, 0, 1}, {AngleFunctions::Form::Harmonic, "k=585.8 eq=121.5"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 2, 3}, {TorsionFunctions::Form::Cosine, "15.178  2  180  1"});
    systemTest.checkSpeciesIntramolecular(sp, {0, 1, 5, 11}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(sp, {1, 0, 2, 6}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(sp, {2, 1, 3, 7}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(sp, {3, 2, 4, 8}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(sp, {4, 3, 5, 9}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(sp, {5, 0, 4, 10}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
}
}; // namespace UnitTest

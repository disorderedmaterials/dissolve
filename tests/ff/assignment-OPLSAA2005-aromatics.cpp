// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(OPLSAA2005AromaticsAssignmentTest, Benzene)
{
    Species species("Benzene");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/benzene.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("OPLSAA2005/Aromatics")));

    ASSERT_EQ(species.nBonds(), 12);
    ASSERT_EQ(species.nAngles(), 18);
    ASSERT_EQ(species.nTorsions(), 24);
    ASSERT_EQ(species.nImpropers(), 6);

    systemTest.checkSpeciesAtomType(&species, {{0, "CA"}, {1, "HA"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3071.06 eq=1.08"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 2}, {BondFunctions::Form::Harmonic, "k=3924.59 eq=1.4"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=292.88 eq=120"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 2, 4}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2, 3}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2, 4}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
    systemTest.checkSpeciesIntramolecular(&species, {10, 0, 2, 4}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
}

TEST(OPLSAA2005AromaticsAssignmentTest, Naphthalene)
{
    Species species("Naphthalene");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/naphthalene.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("OPLSAA2005/Aromatics")));

    ASSERT_EQ(species.nBonds(), 19);
    ASSERT_EQ(species.nAngles(), 30);
    ASSERT_EQ(species.nTorsions(), 44);
    ASSERT_EQ(species.nImpropers(), 10);

    systemTest.checkSpeciesAtomType(&species, {{0, "CA"},
                                               {1, "CA"},
                                               {2, "CNap"},
                                               {3, "CA"},
                                               {4, "CA"},
                                               {5, "CA"},
                                               {6, "CA"},
                                               {7, "CNap"},
                                               {8, "CA"},
                                               {9, "CA"},
                                               {10, "HA"},
                                               {11, "HA"},
                                               {12, "HA"},
                                               {13, "HA"},
                                               {14, "HA"},
                                               {15, "HA"},
                                               {16, "HA"},
                                               {17, "HA"}});
    systemTest.checkSpeciesIntramolecular(&species, {2, 7}, {BondFunctions::Form::Harmonic, "k=3924.59 eq=1.4"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 2, 7}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 2, 3}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    systemTest.checkSpeciesIntramolecular(&species, {8, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
}
}; // namespace UnitTest

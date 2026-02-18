// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(OPLSAA2005AlcoholsAssignmentTest, Methanol)
{
    Species species("Methanol");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/methanol.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("OPLSAA2005/Alcohols"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 5);
    ASSERT_EQ(species.nAngles(), 7);
    ASSERT_EQ(species.nTorsions(), 3);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "CT"}, {1, "OH"}, {2, "HO"}, {3, "HC"}, {4, "HC"}, {5, "HC"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=2677.76 eq=1.41"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=460.24 eq=108.5"});
    systemTest.checkSpeciesIntramolecular(&species, {4, 0, 1}, {AngleFunctions::Form::Harmonic, "k=292.88 eq=109.5"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.47444"});
}
}; // namespace UnitTest

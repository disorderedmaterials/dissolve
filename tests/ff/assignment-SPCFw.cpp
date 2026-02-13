// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SPCFwAssignmentTest, Water)
{
    Species species("Water");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/water.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms(1.2);
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("SPC/Fw"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 2);
    ASSERT_EQ(species.nAngles(), 1);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "OW"}, {1, "HW"}, {2, "HW"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4431.53 eq=1"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=317.5656 eq=113.24"});
}
}; // namespace UnitTest

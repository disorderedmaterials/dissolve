// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(OPLSAA2005AlkanesAssignmentTest, Heptane)
{
    Species species("Heptane");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/heptane.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("OPLSAA2005/Alkanes"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 16);
    ASSERT_EQ(species.nAngles(), 30);
    ASSERT_EQ(species.nTorsions(), 36);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CT3");
    systemTest.checkSpeciesAtomType(&species, 1, "CT2");
    systemTest.checkSpeciesAtomType(&species, 2, "CT2");
    systemTest.checkSpeciesAtomType(&species, 3, "CT2");
    systemTest.checkSpeciesAtomType(&species, 4, "CT3");
    systemTest.checkSpeciesAtomType(&species, 5, "HC");
    systemTest.checkSpeciesAtomType(&species, 6, "HC");
    systemTest.checkSpeciesAtomType(&species, 7, "HC");
    systemTest.checkSpeciesAtomType(&species, 8, "HC");
    systemTest.checkSpeciesAtomType(&species, 9, "HC");
    systemTest.checkSpeciesAtomType(&species, 10, "HC");
    systemTest.checkSpeciesAtomType(&species, 11, "HC");
    systemTest.checkSpeciesAtomType(&species, 12, "HC");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "HC");
    systemTest.checkSpeciesAtomType(&species, 16, "HC");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=2242.62 eq=1.529"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 7}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 9}, {AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=488.273 eq=112.7"});
    systemTest.checkSpeciesIntramolecular(&species, {15, 4, 16}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    systemTest.checkSpeciesIntramolecular(&species, {7, 0, 1, 9}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(&species, {7, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2, 3}, {TorsionFunctions::Form::Cos3, "5.4392  -0.2092  0.8368"});
}

TEST(OPLSAA2005AlkanesAssignmentTest, Cycloheptane)
{
    Species species("Cycloheptane");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/cycloheptane.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("OPLSAA2005/Alkanes"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 21);
    ASSERT_EQ(species.nAngles(), 42);
    ASSERT_EQ(species.nTorsions(), 63);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "CT2");
    systemTest.checkSpeciesAtomType(&species, 1, "CT2");
    systemTest.checkSpeciesAtomType(&species, 2, "CT2");
    systemTest.checkSpeciesAtomType(&species, 3, "CT2");
    systemTest.checkSpeciesAtomType(&species, 4, "CT2");
    systemTest.checkSpeciesAtomType(&species, 5, "CT2");
    systemTest.checkSpeciesAtomType(&species, 6, "CT2");
    systemTest.checkSpeciesAtomType(&species, 7, "HC");
    systemTest.checkSpeciesAtomType(&species, 8, "HC");
    systemTest.checkSpeciesAtomType(&species, 9, "HC");
    systemTest.checkSpeciesAtomType(&species, 10, "HC");
    systemTest.checkSpeciesAtomType(&species, 11, "HC");
    systemTest.checkSpeciesAtomType(&species, 12, "HC");
    systemTest.checkSpeciesAtomType(&species, 13, "HC");
    systemTest.checkSpeciesAtomType(&species, 14, "HC");
    systemTest.checkSpeciesAtomType(&species, 15, "HC");
    systemTest.checkSpeciesAtomType(&species, 16, "HC");
    systemTest.checkSpeciesAtomType(&species, 17, "HC");
    systemTest.checkSpeciesAtomType(&species, 18, "HC");
    systemTest.checkSpeciesAtomType(&species, 19, "HC");
    systemTest.checkSpeciesAtomType(&species, 20, "HC");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=2242.62 eq=1.529"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 7}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 9}, {AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=488.273 eq=112.7"});
    systemTest.checkSpeciesIntramolecular(&species, {15, 4, 16}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    systemTest.checkSpeciesIntramolecular(&species, {7, 0, 1, 9}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(&species, {7, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.2552"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2, 3}, {TorsionFunctions::Form::Cos3, "5.4392  -0.2092  0.8368"});
}
}; // namespace UnitTest

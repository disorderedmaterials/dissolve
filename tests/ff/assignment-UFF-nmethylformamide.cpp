// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(UFFNMethylFormamideAssignmentTest, NMethylFormamide)
{
    Species species("NMethylFormamide");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/n-methylformamide.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("UFF"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 8);
    ASSERT_EQ(species.nAngles(), 12);
    ASSERT_EQ(species.nTorsions(), 10);
    ASSERT_EQ(species.nImpropers(), 2);

    systemTest.checkSpeciesAtomType(
        &species, {{0, "H_"}, {1, "C_amR"}, {2, "N_amR"}, {3, "O_2"}, {4, "C_3"}, {5, "H_"}, {6, "H_"}, {7, "H_"}, {8, "H_"}});
    systemTest.checkSpeciesIntramolecular(&species, {1, 2}, {BondFunctions::Form::Harmonic, "k=5410.2978 eq=1.3568"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=2991.0611 eq=1.08142"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {2, 4}, {BondFunctions::Form::Harmonic, "k=4432.3881 eq=1.45007"}, 5.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {1, 2, 4}, {AngleFunctions::Form::Cos2, "882.7151 0.5 0.666667 0.333333"},
                                          5.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {3, 1, 2}, {AngleFunctions::Form::Cos2, "1590.3058 0.5 0.6667 0.3333"},
                                          5.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {8, 4, 7},
                                          {AngleFunctions::Form::Cos2, "315.8868 0.343737 0.37492 0.281246"}, 6.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2, 4}, {TorsionFunctions::Form::UFFCosine, "101.9308  2  180"},
                                          2.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2, 3}, {TorsionFunctions::Form::FourierN, "209.2 1 -1 0"});
    systemTest.checkSpeciesIntramolecular(&species, {2, 5, 1, 4}, {TorsionFunctions::Form::FourierN, "25.104 1 -1 0"});
}
}; // namespace UnitTest

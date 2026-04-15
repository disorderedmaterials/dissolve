// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(UFF4MOFMOF5AssignmentTest, MOF5)
{
    Species species("MOF5");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/mof5.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.createBox({25.8320, 25.8320, 25.8320}, {90, 90, 90});
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("UFF4MOF")));

    ASSERT_EQ(species.nBonds(), 512);
    ASSERT_EQ(species.nAngles(), 912);
    ASSERT_EQ(species.nTorsions(), 1536);
    ASSERT_EQ(species.nImpropers(), 192);

    systemTest.checkSpeciesAtomType(&species, {{0, "Zn3f2"},
                                               {1, "Zn3f2"},
                                               {26, "Zn3f2"},
                                               {29, "Zn3f2"},
                                               {32, "O_f_3"},
                                               {71, "O_m_2"},
                                               {112, "O_m_2"},
                                               {117, "O_m_2"},
                                               {121, "O_m_2"},
                                               {133, "O_m_2"},
                                               {122, "O_m_2"},
                                               {156, "C_R"},
                                               {178, "C_R"},
                                               {226, "C_R"},
                                               {290, "C_R"},
                                               {386, "H_"}});
    systemTest.checkSpeciesIntramolecular(&species, {386, 290}, {BondFunctions::Form::Harmonic, "k=2991.0611 eq=1.08142"},
                                          1.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {0, 32}, {BondFunctions::Form::Harmonic, "k=1337.872 eq=1.84185"}, 3.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {8, 32}, {BondFunctions::Form::Harmonic, "k=1337.872 eq=1.84185"}, 1.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {71, 156}, {BondFunctions::Form::Harmonic, "k=5048.4039 eq=1.34262"},
                                          5.0e-6);
    systemTest.checkSpeciesIntramolecular(&species, {178, 122, 26}, {AngleFunctions::Form::Cosine, "53.5872 3 0 -1"}, 3.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {121, 29, 112}, {AngleFunctions::Form::Cosine, "107.58286 3 0 -1"}, 6.0e-6);
    systemTest.checkSpeciesIntramolecular(&species, {178, 122, 26, 117}, {TorsionFunctions::Form::UFFCosine, "96.4708  2  180"},
                                          5.0e-5);
    systemTest.checkSpeciesIntramolecular(&species, {178, 122, 133, 226}, {TorsionFunctions::Form::FourierN, "25.104 1 -1 0"});
}
}; // namespace UnitTest

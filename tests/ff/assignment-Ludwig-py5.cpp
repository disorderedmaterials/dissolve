// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
TEST(LudwigPy5AssignmentTest, Py5)
{
    Species species("Py5");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/py5.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Ludwig/Py5"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 27);
    ASSERT_EQ(species.nAngles(), 48);
    ASSERT_EQ(species.nTorsions(), 66);
    ASSERT_EQ(species.nImpropers(), 6);

    systemTest.checkSpeciesAtomType(&species, 0, "nc");
    systemTest.checkSpeciesAtomType(&species, 1, "ca_o");
    systemTest.checkSpeciesAtomType(&species, 2, "ca_m");
    systemTest.checkSpeciesAtomType(&species, 3, "ca_p");
    systemTest.checkSpeciesAtomType(&species, 4, "ca_m");
    systemTest.checkSpeciesAtomType(&species, 5, "ca_o");
    systemTest.checkSpeciesAtomType(&species, 6, "ha_o");
    systemTest.checkSpeciesAtomType(&species, 7, "ha_m");
    systemTest.checkSpeciesAtomType(&species, 8, "ha_p");
    systemTest.checkSpeciesAtomType(&species, 9, "ha_m");
    systemTest.checkSpeciesAtomType(&species, 10, "ha_o");
    systemTest.checkSpeciesAtomType(&species, 11, "ct_1");
    systemTest.checkSpeciesAtomType(&species, 12, "ct_2");
    systemTest.checkSpeciesAtomType(&species, 13, "hc_1");
    systemTest.checkSpeciesAtomType(&species, 14, "hc_1");
    systemTest.checkSpeciesAtomType(&species, 15, "ct_3");
    systemTest.checkSpeciesAtomType(&species, 16, "hc_2");
    systemTest.checkSpeciesAtomType(&species, 17, "hc_2");
    systemTest.checkSpeciesAtomType(&species, 18, "ct_4");
    systemTest.checkSpeciesAtomType(&species, 19, "hc_3");
    systemTest.checkSpeciesAtomType(&species, 20, "hc_3");
    systemTest.checkSpeciesAtomType(&species, 21, "cm");
    systemTest.checkSpeciesAtomType(&species, 22, "hc_4");
    systemTest.checkSpeciesAtomType(&species, 23, "hc_4");
    systemTest.checkSpeciesAtomType(&species, 24, "hm");
    systemTest.checkSpeciesAtomType(&species, 25, "hm");
    systemTest.checkSpeciesAtomType(&species, 26, "hm");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4042 eq=1.339"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 6}, {BondFunctions::Form::Harmonic, "k=3071 eq=1.08"});
    systemTest.checkSpeciesIntramolecular(&species, {11, 0, 1}, {AngleFunctions::Form::Harmonic, "k=585.8 eq=121.5"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 2, 3}, {TorsionFunctions::Form::Cosine, "15.178  2  180  1"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1, 5, 11}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2, 6}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(&species, {2, 1, 3, 7}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 2, 4, 8}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(&species, {4, 3, 5, 9}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    systemTest.checkSpeciesIntramolecular(&species, {5, 0, 4, 10}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
}
}; // namespace UnitTest

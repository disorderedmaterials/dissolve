// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(Kulmala2010ForcefieldTest, Hydronium)
{
    Species species("Hydronium");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/hydronium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 3);
    ASSERT_EQ(species.nAngles(), 3);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "OW+");
    systemTest.checkSpeciesAtomType(&species, 1, "HW+");
    systemTest.checkSpeciesAtomType(&species, 2, "HW+");
    systemTest.checkSpeciesAtomType(&species, 3, "HW+");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4030 eq=0.969"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=451.4 eq=113.1"});
}

TEST(Kulmala2010ForcefieldTest, Ammonia)
{
    Species species("Ammonia");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/ammonia.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 3);
    ASSERT_EQ(species.nAngles(), 3);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "NH");
    systemTest.checkSpeciesAtomType(&species, 1, "HN");
    systemTest.checkSpeciesAtomType(&species, 2, "HN");
    systemTest.checkSpeciesAtomType(&species, 3, "HN");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3864 eq=1.003"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=334.01 eq=107.18"});
}

TEST(Kulmala2010ForcefieldTest, Ammonium)
{
    Species species("Ammonium");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/ammonium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 4);
    ASSERT_EQ(species.nAngles(), 6);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "NH+");
    systemTest.checkSpeciesAtomType(&species, 1, "HN+");
    systemTest.checkSpeciesAtomType(&species, 2, "HN+");
    systemTest.checkSpeciesAtomType(&species, 3, "HN+");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3660.3 eq=1.013"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=518.26 eq=109.47"});
}

TEST(Kulmala2010ForcefieldTest, Dimethylammonium)
{
    Species species("Dimethylammonium");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/dimethylammonium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 10);
    ASSERT_EQ(species.nAngles(), 18);
    ASSERT_EQ(species.nTorsions(), 18);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "NDM");
    systemTest.checkSpeciesAtomType(&species, 1, "HDM");
    systemTest.checkSpeciesAtomType(&species, 2, "HDM");
    systemTest.checkSpeciesAtomType(&species, 3, "CDM");
    systemTest.checkSpeciesAtomType(&species, 4, "CDM");
    systemTest.checkSpeciesAtomType(&species, 5, "HCD");
    systemTest.checkSpeciesAtomType(&species, 6, "HCD");
    systemTest.checkSpeciesAtomType(&species, 7, "HCD");
    systemTest.checkSpeciesAtomType(&species, 8, "HCD");
    systemTest.checkSpeciesAtomType(&species, 9, "HCD");
    systemTest.checkSpeciesAtomType(&species, 10, "HCD");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3632 eq=1.01"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=3071 eq=1.499"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 8}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    systemTest.checkSpeciesIntramolecular(&species, {2, 0, 1}, {AngleFunctions::Form::Harmonic, "k=292.9 eq=105.5"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 0, 1}, {AngleFunctions::Form::Harmonic, "k=418.4 eq=109.2"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 3, 8}, {AngleFunctions::Form::Harmonic, "k=209.2 eq=108.5"});
    systemTest.checkSpeciesIntramolecular(&species, {9, 3, 8}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    systemTest.checkSpeciesIntramolecular(&species, {2, 0, 4, 5}, {TorsionFunctions::Form::Cos3, "0  0  0"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 0, 4, 5}, {TorsionFunctions::Form::Cos3, "0  0  3.3765"});
}

TEST(Kulmala2010ForcefieldTest, H2SO4)
{
    Species species("H2SO4");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/h2so4.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 6);
    ASSERT_EQ(species.nAngles(), 8);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "SA");
    systemTest.checkSpeciesAtomType(&species, 1, "OA");
    systemTest.checkSpeciesAtomType(&species, 2, "OA");
    systemTest.checkSpeciesAtomType(&species, 3, "OHA");
    systemTest.checkSpeciesAtomType(&species, 4, "OHA");
    systemTest.checkSpeciesAtomType(&species, 5, "HSA");
    systemTest.checkSpeciesAtomType(&species, 6, "HSA");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=6258 eq=1.411"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=3083.8 eq=1.571"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 6}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=224.37 eq=123.69"});
    systemTest.checkSpeciesIntramolecular(&species, {6, 3, 0}, {AngleFunctions::Form::Harmonic, "k=396.82 eq=110.5"});
    systemTest.checkSpeciesIntramolecular(&species, {6, 3, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

TEST(Kulmala2010ForcefieldTest, HSO4Minus)
{
    Species species("HSO4-");
    CoreData removeMeCoreData_;
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/hso4minus.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010"), removeMeCoreData_));

    ASSERT_EQ(species.nBonds(), 5);
    ASSERT_EQ(species.nAngles(), 7);
    ASSERT_EQ(species.nTorsions(), 3);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, 0, "SA-");
    systemTest.checkSpeciesAtomType(&species, 1, "OA-");
    systemTest.checkSpeciesAtomType(&species, 2, "OA-");
    systemTest.checkSpeciesAtomType(&species, 3, "OA-");
    systemTest.checkSpeciesAtomType(&species, 4, "OHA-");
    systemTest.checkSpeciesAtomType(&species, 5, "HSA-");
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=5239.9 eq=1.438"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 4}, {BondFunctions::Form::Harmonic, "k=2328.3 eq=1.634"});
    systemTest.checkSpeciesIntramolecular(&species, {4, 5}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=456.97 eq=114.76"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 4}, {AngleFunctions::Form::Harmonic, "k=814.48 eq=103.45"});
    systemTest.checkSpeciesIntramolecular(&species, {5, 4, 0}, {AngleFunctions::Form::Harmonic, "k=398.12 eq=106.39"});
    systemTest.checkSpeciesIntramolecular(&species, {5, 4, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "io/import/species.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(Kulmala2010AssignmentTest, Hydronium)
{
    Species species("Hydronium");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/hydronium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010")));

    ASSERT_EQ(species.nBonds(), 3);
    ASSERT_EQ(species.nAngles(), 3);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "OW+"}, {1, "HW+"}, {2, "HW+"}, {3, "HW+"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4030 eq=0.969"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=451.4 eq=113.1"});
}

TEST(Kulmala2010AssignmentTest, Ammonia)
{
    Species species("Ammonia");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/ammonia.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010")));

    ASSERT_EQ(species.nBonds(), 3);
    ASSERT_EQ(species.nAngles(), 3);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "NH"}, {1, "HN"}, {2, "HN"}, {3, "HN"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3864 eq=1.003"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=334.01 eq=107.18"});
}

TEST(Kulmala2010AssignmentTest, Ammonium)
{
    Species species("Ammonium");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/ammonium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010")));

    ASSERT_EQ(species.nBonds(), 4);
    ASSERT_EQ(species.nAngles(), 6);
    ASSERT_EQ(species.nTorsions(), 0);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "NH+"}, {1, "HN+"}, {2, "HN+"}, {3, "HN+"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3660.3 eq=1.013"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=518.26 eq=109.47"});
}

TEST(Kulmala2010AssignmentTest, Dimethylammonium)
{
    Species species("Dimethylammonium");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/dimethylammonium.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010")));

    ASSERT_EQ(species.nBonds(), 10);
    ASSERT_EQ(species.nAngles(), 18);
    ASSERT_EQ(species.nTorsions(), 18);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "NDM"},
                                               {1, "HDM"},
                                               {2, "HDM"},
                                               {3, "CDM"},
                                               {4, "CDM"},
                                               {5, "HCD"},
                                               {6, "HCD"},
                                               {7, "HCD"},
                                               {8, "HCD"},
                                               {9, "HCD"},
                                               {10, "HCD"}});
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

TEST(Kulmala2010AssignmentTest, H2SO4)
{
    Species species("H2SO4");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/h2so4.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010")));

    ASSERT_EQ(species.nBonds(), 6);
    ASSERT_EQ(species.nAngles(), 8);
    ASSERT_EQ(species.nTorsions(), 6);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species,
                                    {{0, "SA"}, {1, "OA"}, {2, "OA"}, {3, "OHA"}, {4, "OHA"}, {5, "HSA"}, {6, "HSA"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=6258 eq=1.411"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=3083.8 eq=1.571"});
    systemTest.checkSpeciesIntramolecular(&species, {3, 6}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=224.37 eq=123.69"});
    systemTest.checkSpeciesIntramolecular(&species, {6, 3, 0}, {AngleFunctions::Form::Harmonic, "k=396.82 eq=110.5"});
    systemTest.checkSpeciesIntramolecular(&species, {6, 3, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

TEST(Kulmala2010AssignmentTest, HSO4Minus)
{
    Species species("HSO4-");
    DissolveSystemTest systemTest;
    SpeciesImportFileFormat importer("xyz/hso4minus.xyz");
    ASSERT_TRUE(importer.importData(&species));
    species.recalculateIntermolecularTerms();
    ASSERT_TRUE(species.applyForcefieldTerms(ForcefieldLibrary::forcefield("Kulmala2010")));

    ASSERT_EQ(species.nBonds(), 5);
    ASSERT_EQ(species.nAngles(), 7);
    ASSERT_EQ(species.nTorsions(), 3);
    ASSERT_EQ(species.nImpropers(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "SA-"}, {1, "OA-"}, {2, "OA-"}, {3, "OA-"}, {4, "OHA-"}, {5, "HSA-"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=5239.9 eq=1.438"});
    systemTest.checkSpeciesIntramolecular(&species, {0, 4}, {BondFunctions::Form::Harmonic, "k=2328.3 eq=1.634"});
    systemTest.checkSpeciesIntramolecular(&species, {4, 5}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=456.97 eq=114.76"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 4}, {AngleFunctions::Form::Harmonic, "k=814.48 eq=103.45"});
    systemTest.checkSpeciesIntramolecular(&species, {5, 4, 0}, {AngleFunctions::Form::Harmonic, "k=398.12 eq=106.39"});
    systemTest.checkSpeciesIntramolecular(&species, {5, 4, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(Kulmala2010AssignmentTest, Hydronium)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield(
        "Hydronium", "ImportXYZStructure", "xyz/hydronium.xyz", ForcefieldLibrary::forcefield("Kulmala2010"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 3);
    ASSERT_EQ(species.angles().size(), 3);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "OW+"}, {1, "HW+"}, {2, "HW+"}, {3, "HW+"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4030 eq=0.969"});
    testSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=451.4 eq=113.1"});
}

TEST(Kulmala2010AssignmentTest, Ammonia)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Ammonia", "ImportXYZStructure", "xyz/ammonia.xyz",
                                                                          ForcefieldLibrary::forcefield("Kulmala2010"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 3);
    ASSERT_EQ(species.angles().size(), 3);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "NH"}, {1, "HN"}, {2, "HN"}, {3, "HN"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3864 eq=1.003"});
    testSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=334.01 eq=107.18"});
}

TEST(Kulmala2010AssignmentTest, Ammonium)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Ammonium", "ImportXYZStructure", "xyz/ammonium.xyz",
                                                                          ForcefieldLibrary::forcefield("Kulmala2010"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 4);
    ASSERT_EQ(species.angles().size(), 6);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "NH+"}, {1, "HN+"}, {2, "HN+"}, {3, "HN+"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3660.3 eq=1.013"});
    testSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=518.26 eq=109.47"});
}

TEST(Kulmala2010AssignmentTest, Dimethylammonium)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield(
        "Dimethylammonium", "ImportXYZStructure", "xyz/dimethylammonium.xyz", ForcefieldLibrary::forcefield("Kulmala2010"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 10);
    ASSERT_EQ(species.angles().size(), 18);
    ASSERT_EQ(species.torsions().size(), 18);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "NDM"},
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
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3632 eq=1.01"});
    testSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=3071 eq=1.499"});
    testSpeciesIntramolecular(&species, {3, 8}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    testSpeciesIntramolecular(&species, {2, 0, 1}, {AngleFunctions::Form::Harmonic, "k=292.9 eq=105.5"});
    testSpeciesIntramolecular(&species, {3, 0, 1}, {AngleFunctions::Form::Harmonic, "k=418.4 eq=109.2"});
    testSpeciesIntramolecular(&species, {0, 3, 8}, {AngleFunctions::Form::Harmonic, "k=209.2 eq=108.5"});
    testSpeciesIntramolecular(&species, {9, 3, 8}, {AngleFunctions::Form::Harmonic, "k=276.144 eq=107.8"});
    testSpeciesIntramolecular(&species, {2, 0, 4, 5}, {TorsionFunctions::Form::Cos3, "0  0  0"});
    testSpeciesIntramolecular(&species, {3, 0, 4, 5}, {TorsionFunctions::Form::Cos3, "0  0  3.3765"});
}

TEST(Kulmala2010AssignmentTest, H2SO4)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("H2SO4", "ImportXYZStructure", "xyz/h2so4.xyz",
                                                                          ForcefieldLibrary::forcefield("Kulmala2010"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 6);
    ASSERT_EQ(species.angles().size(), 8);
    ASSERT_EQ(species.torsions().size(), 6);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "SA"}, {1, "OA"}, {2, "OA"}, {3, "OHA"}, {4, "OHA"}, {5, "HSA"}, {6, "HSA"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=6258 eq=1.411"});
    testSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=3083.8 eq=1.571"});
    testSpeciesIntramolecular(&species, {3, 6}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    testSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=224.37 eq=123.69"});
    testSpeciesIntramolecular(&species, {6, 3, 0}, {AngleFunctions::Form::Harmonic, "k=396.82 eq=110.5"});
    testSpeciesIntramolecular(&species, {6, 3, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

TEST(Kulmala2010AssignmentTest, HSO4Minus)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("HSO4-", "ImportXYZStructure", "xyz/hso4minus.xyz",
                                                                          ForcefieldLibrary::forcefield("Kulmala2010"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 5);
    ASSERT_EQ(species.angles().size(), 7);
    ASSERT_EQ(species.torsions().size(), 3);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "SA-"}, {1, "OA-"}, {2, "OA-"}, {3, "OA-"}, {4, "OHA-"}, {5, "HSA-"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=5239.9 eq=1.438"});
    testSpeciesIntramolecular(&species, {0, 4}, {BondFunctions::Form::Harmonic, "k=2328.3 eq=1.634"});
    testSpeciesIntramolecular(&species, {4, 5}, {BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949"});
    testSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=456.97 eq=114.76"});
    testSpeciesIntramolecular(&species, {1, 0, 4}, {AngleFunctions::Form::Harmonic, "k=814.48 eq=103.45"});
    testSpeciesIntramolecular(&species, {5, 4, 0}, {AngleFunctions::Form::Harmonic, "k=398.12 eq=106.39"});
    testSpeciesIntramolecular(&species, {5, 4, 0, 2}, {TorsionFunctions::Form::Cos3, "0  0  0"});
}

} // namespace UnitTest

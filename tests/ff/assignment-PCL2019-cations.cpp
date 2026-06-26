// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(PCL2019CationsAssignmentTest, benzc1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("benzc1im", "ImportXYZStructure", "xyz/benzc1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 27);
    ASSERT_EQ(species.angles().size(), 45);
    ASSERT_EQ(species.torsions().size(), 62);
    ASSERT_EQ(species.impropers().size(), 11);

    checkSpeciesAtomType(&species, {{0, "NA"},   {1, "CR"},   {2, "NA"},   {3, "CW"},   {4, "CW"},   {5, "C1"},   {6, "HCR"},
                                    {7, "C1T"},  {8, "HCW"},  {9, "HCW"},  {10, "H1"},  {11, "H1"},  {12, "H1"},  {13, "H1"},
                                    {14, "H1"},  {15, "CAT"}, {16, "CAO"}, {17, "CAM"}, {18, "CAP"}, {19, "CAM"}, {20, "CAO"},
                                    {21, "HAT"}, {22, "HAT"}, {23, "HAT"}, {24, "HAT"}, {25, "HAT"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c12c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c12c1im", "ImportXYZStructure", "xyz/c12c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 49);
    ASSERT_EQ(species.angles().size(), 93);
    ASSERT_EQ(species.torsions().size(), 131);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"},
                                    {7, "C1"},  {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"},
                                    {14, "H1"}, {15, "H1"}, {16, "CS"}, {17, "HC"}, {18, "HC"}, {19, "CS"}, {20, "HC"},
                                    {21, "HC"}, {22, "CS"}, {23, "HC"}, {24, "HC"}, {25, "CS"}, {26, "HC"}, {27, "HC"},
                                    {28, "CS"}, {29, "HC"}, {30, "HC"}, {31, "CS"}, {32, "HC"}, {33, "HC"}, {34, "CS"},
                                    {35, "HC"}, {36, "HC"}, {37, "CS"}, {38, "HC"}, {39, "HC"}, {40, "CS"}, {41, "HC"},
                                    {42, "HC"}, {43, "CT"}, {44, "HC"}, {45, "HC"}, {46, "HC"}, {47, "HC"}, {48, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c1c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c1c1im", "ImportXYZStructure", "xyz/c1c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 16);
    ASSERT_EQ(species.angles().size(), 27);
    ASSERT_EQ(species.torsions().size(), 32);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NA"},
                                    {1, "CR"},
                                    {2, "NA"},
                                    {3, "CW"},
                                    {4, "CW"},
                                    {5, "C1"},
                                    {6, "HCR"},
                                    {7, "C1"},
                                    {8, "HCW"},
                                    {9, "HCW"},
                                    {10, "H1"},
                                    {11, "H1"},
                                    {12, "H1"},
                                    {13, "H1"},
                                    {14, "H1"},
                                    {15, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c1c1pyrr)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c1c1pyrr", "ImportXYZStructure", "xyz/c1c1pyrr.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 21);
    ASSERT_EQ(species.angles().size(), 42);
    ASSERT_EQ(species.torsions().size(), 63);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species, {{0, "N4"},  {1, "C1"},  {2, "C1"},  {3, "C2"},  {4, "C2"},  {5, "C1"},  {6, "C1"},
                                    {7, "H1"},  {8, "H1"},  {9, "H1"},  {10, "H1"}, {11, "HC"}, {12, "HC"}, {13, "HC"},
                                    {14, "HC"}, {15, "H1"}, {16, "H1"}, {17, "H1"}, {18, "H1"}, {19, "H1"}, {20, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c2c1c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c2c1c1im", "ImportXYZStructure", "xyz/c2c1c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 22);
    ASSERT_EQ(species.angles().size(), 39);
    ASSERT_EQ(species.torsions().size(), 47);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species,
                         {{0, "NAM"}, {1, "CRM"}, {2, "NAM"}, {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "CCR"}, {7, "C1"},
                          {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "CE"}, {14, "H1"}, {15, "H1"},
                          {16, "HC"}, {17, "HC"}, {18, "HC"}, {19, "H1"}, {20, "H1"}, {21, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c2c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c2c1im", "ImportXYZStructure", "xyz/c2c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 19);
    ASSERT_EQ(species.angles().size(), 33);
    ASSERT_EQ(species.torsions().size(), 41);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NA"},
                                    {1, "CR"},
                                    {2, "NA"},
                                    {3, "CW"},
                                    {4, "CW"},
                                    {5, "C1"},
                                    {6, "HCR"},
                                    {7, "C1"},
                                    {8, "HCW"},
                                    {9, "HCW"},
                                    {10, "H1"},
                                    {11, "H1"},
                                    {12, "H1"},
                                    {13, "CE"},
                                    {14, "H1"},
                                    {15, "H1"},
                                    {16, "HC"},
                                    {17, "HC"},
                                    {18, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c2im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c2im", "ImportXYZStructure", "xyz/c2im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 16);
    ASSERT_EQ(species.angles().size(), 27);
    ASSERT_EQ(species.torsions().size(), 35);
    ASSERT_EQ(species.impropers().size(), 4);

    checkSpeciesAtomType(&species, {{0, "NAH"},
                                    {1, "CRH"},
                                    {2, "NA"},
                                    {3, "CW"},
                                    {4, "CWH"},
                                    {5, "HCR"},
                                    {6, "C1"},
                                    {7, "HCW"},
                                    {8, "HCW"},
                                    {9, "CE"},
                                    {10, "H1"},
                                    {11, "H1"},
                                    {12, "HC"},
                                    {13, "HC"},
                                    {14, "HC"},
                                    {15, "HNA"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c2OHc1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c2OHc1im", "ImportXYZStructure", "xyz/c2ohc1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 20);
    ASSERT_EQ(species.angles().size(), 34);
    ASSERT_EQ(species.torsions().size(), 44);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"},
                                    {7, "C1"},  {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2O"},
                                    {14, "H1"}, {15, "H1"}, {16, "OH"}, {17, "HC"}, {18, "HC"}, {19, "HO"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c2py)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c2py", "ImportXYZStructure", "xyz/c2py.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 18);
    ASSERT_EQ(species.angles().size(), 30);
    ASSERT_EQ(species.torsions().size(), 39);
    ASSERT_EQ(species.impropers().size(), 6);

    checkSpeciesAtomType(&species, {{0, "NAP"},
                                    {1, "CAPO"},
                                    {2, "CAPO"},
                                    {3, "CAPM"},
                                    {4, "HAP"},
                                    {5, "CAPP"},
                                    {6, "HAP"},
                                    {7, "CAPM"},
                                    {8, "HAP"},
                                    {9, "HAP"},
                                    {10, "HAP"},
                                    {11, "C1"},
                                    {12, "CE"},
                                    {13, "HC"},
                                    {14, "HC"},
                                    {15, "HC"},
                                    {16, "H1"},
                                    {17, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c3c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c3c1im", "ImportXYZStructure", "xyz/c3c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 22);
    ASSERT_EQ(species.angles().size(), 39);
    ASSERT_EQ(species.torsions().size(), 50);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species,
                         {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"}, {7, "C1"},
                          {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"}, {14, "H1"}, {15, "H1"},
                          {16, "CT"}, {17, "HC"}, {18, "HC"}, {19, "HC"}, {20, "HC"}, {21, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c3c1pyrr)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c3c1pyrr", "ImportXYZStructure", "xyz/c3c1pyrr.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 27);
    ASSERT_EQ(species.angles().size(), 54);
    ASSERT_EQ(species.torsions().size(), 81);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species, {{0, "N4"},  {1, "C1"},  {2, "C1"},  {3, "C2"},  {4, "C2"},  {5, "C1"},  {6, "C1"},
                                    {7, "H1"},  {8, "H1"},  {9, "H1"},  {10, "H1"}, {11, "HC"}, {12, "HC"}, {13, "HC"},
                                    {14, "HC"}, {15, "H1"}, {16, "H1"}, {17, "H1"}, {18, "H1"}, {19, "H1"}, {20, "C2"},
                                    {21, "HC"}, {22, "HC"}, {23, "CT"}, {24, "HC"}, {25, "HC"}, {26, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c4c1c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c4c1c1im", "ImportXYZStructure", "xyz/c4c1c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 28);
    ASSERT_EQ(species.angles().size(), 51);
    ASSERT_EQ(species.torsions().size(), 65);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NAM"}, {1, "CRM"}, {2, "NAM"}, {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "CCR"},
                                    {7, "C1"},  {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"},
                                    {14, "H1"}, {15, "H1"}, {16, "CS"}, {17, "HC"}, {18, "HC"}, {19, "CT"}, {20, "HC"},
                                    {21, "HC"}, {22, "HC"}, {23, "HC"}, {24, "HC"}, {25, "H1"}, {26, "H1"}, {27, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c4c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c4c1im", "ImportXYZStructure", "xyz/c4c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 25);
    ASSERT_EQ(species.angles().size(), 45);
    ASSERT_EQ(species.torsions().size(), 59);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"},
                                    {7, "C1"},  {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"},
                                    {14, "H1"}, {15, "H1"}, {16, "CS"}, {17, "HC"}, {18, "HC"}, {19, "CT"}, {20, "HC"},
                                    {21, "HC"}, {22, "HC"}, {23, "HC"}, {24, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c4c1pyrr)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c4c1pyrr", "ImportXYZStructure", "xyz/c4c1pyrr.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 30);
    ASSERT_EQ(species.angles().size(), 60);
    ASSERT_EQ(species.torsions().size(), 90);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species,
                         {{0, "N4"},  {1, "C1"},  {2, "C1"},  {3, "C2"},  {4, "C2"},  {5, "C1"},  {6, "C1"},  {7, "H1"},
                          {8, "H1"},  {9, "H1"},  {10, "H1"}, {11, "HC"}, {12, "HC"}, {13, "HC"}, {14, "HC"}, {15, "H1"},
                          {16, "H1"}, {17, "H1"}, {18, "H1"}, {19, "H1"}, {20, "C2"}, {21, "HC"}, {22, "HC"}, {23, "CS"},
                          {24, "HC"}, {25, "HC"}, {26, "CT"}, {27, "HC"}, {28, "HC"}, {29, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c4c4im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c4c4im", "ImportXYZStructure", "xyz/c4c4im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 34);
    ASSERT_EQ(species.angles().size(), 63);
    ASSERT_EQ(species.torsions().size(), 86);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species, {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"},
                                    {7, "C1"},  {8, "HCW"}, {9, "HCW"}, {10, "C2"}, {11, "H1"}, {12, "H1"}, {13, "CS"},
                                    {14, "HC"}, {15, "HC"}, {16, "CT"}, {17, "HC"}, {18, "HC"}, {19, "HC"}, {20, "HC"},
                                    {21, "HC"}, {22, "C2"}, {23, "H1"}, {24, "H1"}, {25, "CS"}, {26, "HC"}, {27, "HC"},
                                    {28, "CT"}, {29, "HC"}, {30, "HC"}, {31, "HC"}, {32, "HC"}, {33, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c4pyri)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c4pyri", "ImportXYZStructure", "xyz/c4pyri.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 24);
    ASSERT_EQ(species.angles().size(), 42);
    ASSERT_EQ(species.torsions().size(), 57);
    ASSERT_EQ(species.impropers().size(), 6);

    checkSpeciesAtomType(&species,
                         {{0, "NAP"}, {1, "CAPO"}, {2, "CAPM"}, {3, "CAPP"}, {4, "CAPM"}, {5, "CAPO"}, {6, "C1"},  {7, "HAP"},
                          {8, "HAP"}, {9, "HAP"},  {10, "HAP"}, {11, "HAP"}, {12, "C2"},  {13, "H1"},  {14, "H1"}, {15, "CS"},
                          {16, "HC"}, {17, "HC"},  {18, "CT"},  {19, "HC"},  {20, "HC"},  {21, "HC"},  {22, "HC"}, {23, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c6c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c6c1im", "ImportXYZStructure", "xyz/c6c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 31);
    ASSERT_EQ(species.angles().size(), 57);
    ASSERT_EQ(species.torsions().size(), 77);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species,
                         {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"}, {7, "C1"},
                          {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"}, {14, "H1"}, {15, "H1"},
                          {16, "CS"}, {17, "HC"}, {18, "HC"}, {19, "CS"}, {20, "HC"}, {21, "HC"}, {22, "CS"}, {23, "HC"},
                          {24, "HC"}, {25, "CT"}, {26, "HC"}, {27, "HC"}, {28, "HC"}, {29, "HC"}, {30, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c8c1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c8c1im", "ImportXYZStructure", "xyz/c8c1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 37);
    ASSERT_EQ(species.angles().size(), 69);
    ASSERT_EQ(species.torsions().size(), 95);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species,
                         {{0, "NA"},  {1, "CR"},  {2, "NA"},  {3, "CW"},  {4, "CW"},  {5, "C1"},  {6, "HCR"}, {7, "C1"},
                          {8, "HCW"}, {9, "HCW"}, {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"}, {14, "H1"}, {15, "H1"},
                          {16, "CS"}, {17, "HC"}, {18, "HC"}, {19, "CS"}, {20, "HC"}, {21, "HC"}, {22, "CS"}, {23, "HC"},
                          {24, "HC"}, {25, "CS"}, {26, "HC"}, {27, "HC"}, {28, "CS"}, {29, "HC"}, {30, "HC"}, {31, "CT"},
                          {32, "HC"}, {33, "HC"}, {34, "HC"}, {35, "HC"}, {36, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c8fc1im)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c8fc1im", "ImportXYZStructure", "xyz/c8fc1im.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 37);
    ASSERT_EQ(species.angles().size(), 69);
    ASSERT_EQ(species.torsions().size(), 95);
    ASSERT_EQ(species.impropers().size(), 5);

    checkSpeciesAtomType(&species,
                         {{0, "NA"},   {1, "CR"},   {2, "NA"},  {3, "CW"},   {4, "CW"},   {5, "C1"},  {6, "HCR"},  {7, "C1H"},
                          {8, "HCW"},  {9, "HCW"},  {10, "H1"}, {11, "H1"},  {12, "H1"},  {13, "C2"}, {14, "H1"},  {15, "H1"},
                          {16, "CFH"}, {17, "HC"},  {18, "HC"}, {19, "CSF"}, {20, "F"},   {21, "F"},  {22, "CSF"}, {23, "F"},
                          {24, "F"},   {25, "CSF"}, {26, "F"},  {27, "F"},   {28, "CSF"}, {29, "F"},  {30, "F"},   {31, "CTF"},
                          {32, "F"},   {33, "F"},   {34, "F"},  {35, "F"},   {36, "F"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, c8isoqui)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("c8isoqui", "ImportXYZStructure", "xyz/c8isoqui.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 43);
    ASSERT_EQ(species.angles().size(), 78);
    ASSERT_EQ(species.torsions().size(), 113);
    ASSERT_EQ(species.impropers().size(), 10);

    checkSpeciesAtomType(&species, {{0, "NAQ"}, {1, "CA"},   {2, "CAQ"},  {3, "CA"},   {4, "CA"},  {5, "CA"},  {6, "CA"},
                                    {7, "CAQ"}, {8, "CA"},   {9, "CA"},   {10, "HAP"}, {11, "HA"}, {12, "HA"}, {13, "HA"},
                                    {14, "HA"}, {15, "HAP"}, {16, "HAP"}, {17, "C1"},  {18, "H1"}, {19, "H1"}, {20, "C2"},
                                    {21, "CS"}, {22, "CS"},  {23, "CS"},  {24, "CS"},  {25, "CS"}, {26, "CT"}, {27, "HC"},
                                    {28, "HC"}, {29, "HC"},  {30, "HC"},  {31, "HC"},  {32, "HC"}, {33, "HC"}, {34, "HC"},
                                    {35, "HC"}, {36, "HC"},  {37, "HC"},  {38, "HC"},  {39, "HC"}, {40, "HC"}, {41, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, cholinium)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield(
        "cholinium", "ImportXYZStructure", "xyz/cholinium.xyz", ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 20);
    ASSERT_EQ(species.angles().size(), 37);
    ASSERT_EQ(species.torsions().size(), 48);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species, {{0, "N4"},  {1, "C1"},   {2, "C1"},  {3, "C1"},  {4, "H1"},  {5, "H1"},  {6, "H1"},
                                    {7, "H1"},  {8, "H1"},   {9, "H1"},  {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C1"},
                                    {14, "H1"}, {15, "COL"}, {16, "H1"}, {17, "HC"}, {18, "HC"}, {19, "OH"}, {20, "HO"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, gua)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Gua", "ImportXYZStructure", "xyz/gua.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 9);
    ASSERT_EQ(species.angles().size(), 12);
    ASSERT_EQ(species.torsions().size(), 12);
    ASSERT_EQ(species.impropers().size(), 4);

    checkSpeciesAtomType(
        &species,
        {{0, "CG"}, {1, "NG"}, {2, "NG"}, {3, "NG"}, {4, "HG"}, {5, "HG"}, {6, "HG"}, {7, "HG"}, {8, "HG"}, {9, "HG"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, N1110)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("N1110", "ImportXYZStructure", "xyz/n1110.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 13);
    ASSERT_EQ(species.angles().size(), 24);
    ASSERT_EQ(species.torsions().size(), 27);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species, {{0, "N3"},
                                    {1, "C1"},
                                    {2, "C1"},
                                    {3, "C1"},
                                    {4, "H3"},
                                    {5, "H1"},
                                    {6, "H1"},
                                    {7, "H1"},
                                    {8, "H1"},
                                    {9, "H1"},
                                    {10, "H1"},
                                    {11, "H1"},
                                    {12, "H1"},
                                    {13, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, N1111)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("N1111", "ImportXYZStructure", "xyz/n1111.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 16);
    ASSERT_EQ(species.angles().size(), 30);
    ASSERT_EQ(species.torsions().size(), 36);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species, {{0, "N4"},
                                    {1, "C1"},
                                    {2, "C1"},
                                    {3, "C1"},
                                    {4, "C1"},
                                    {5, "H1"},
                                    {6, "H1"},
                                    {7, "H1"},
                                    {8, "H1"},
                                    {9, "H1"},
                                    {10, "H1"},
                                    {11, "H1"},
                                    {12, "H1"},
                                    {13, "H1"},
                                    {14, "H1"},
                                    {15, "H1"},
                                    {16, "H1"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, N2220)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("N2220", "ImportXYZStructure", "xyz/n2220.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 22);
    ASSERT_EQ(species.angles().size(), 42);
    ASSERT_EQ(species.torsions().size(), 54);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species,
                         {{0, "N3"},  {1, "C1"},  {2, "C1"},  {3, "C1"},  {4, "CE"},  {5, "H1"},  {6, "H1"},  {7, "HC"},
                          {8, "HC"},  {9, "HC"},  {10, "H1"}, {11, "H1"}, {12, "CE"}, {13, "HC"}, {14, "HC"}, {15, "HC"},
                          {16, "H1"}, {17, "H1"}, {18, "CE"}, {19, "HC"}, {20, "HC"}, {21, "HC"}, {22, "H3"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, N2222)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("N2222", "ImportXYZStructure", "xyz/n2222.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 28);
    ASSERT_EQ(species.angles().size(), 54);
    ASSERT_EQ(species.torsions().size(), 72);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(&species,
                         {{0, "N4"},  {1, "C1"},  {2, "C1"},  {3, "C1"},  {4, "C1"},  {5, "H1"},  {6, "H1"},  {7, "H1"},
                          {8, "H1"},  {9, "H1"},  {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "CE"}, {14, "CE"}, {15, "CE"},
                          {16, "CE"}, {17, "HC"}, {18, "HC"}, {19, "HC"}, {20, "HC"}, {21, "HC"}, {22, "HC"}, {23, "HC"},
                          {24, "HC"}, {25, "HC"}, {26, "HC"}, {27, "HC"}, {28, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, N4444)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("N4444", "ImportXYZStructure", "xyz/n4444.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 52);
    ASSERT_EQ(species.angles().size(), 102);
    ASSERT_EQ(species.torsions().size(), 144);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(
        &species, {{0, "N4"},  {1, "C1"},  {2, "C1"},  {3, "C1"},  {4, "C1"},  {5, "H1"},  {6, "H1"},  {7, "H1"},  {8, "H1"},
                   {9, "H1"},  {10, "H1"}, {11, "H1"}, {12, "H1"}, {13, "C2"}, {14, "C2"}, {15, "C2"}, {16, "C2"}, {17, "HC"},
                   {18, "HC"}, {19, "HC"}, {20, "HC"}, {21, "HC"}, {22, "HC"}, {23, "HC"}, {24, "HC"}, {25, "CS"}, {26, "CS"},
                   {27, "CS"}, {28, "CS"}, {29, "CT"}, {30, "CT"}, {31, "CT"}, {32, "CT"}, {33, "HC"}, {34, "HC"}, {35, "HC"},
                   {36, "HC"}, {37, "HC"}, {38, "HC"}, {39, "HC"}, {40, "HC"}, {41, "HC"}, {42, "HC"}, {43, "HC"}, {44, "HC"},
                   {45, "HC"}, {46, "HC"}, {47, "HC"}, {48, "HC"}, {49, "HC"}, {50, "HC"}, {51, "HC"}, {52, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}

TEST(PCL2019CationsAssignmentTest, P66614)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("P66614", "ImportXYZStructure", "xyz/p66614.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Cations"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 100);
    ASSERT_EQ(species.angles().size(), 198);
    ASSERT_EQ(species.torsions().size(), 288);
    ASSERT_EQ(species.impropers().size(), 0);

    checkSpeciesAtomType(
        &species,
        {{0, "P4"},  {1, "C1P"}, {2, "C1P"}, {3, "C1P"}, {4, "C1P"}, {5, "C2"},  {6, "H1"},  {7, "H1"},  {8, "H1"},  {9, "H1"},
         {10, "C2"}, {11, "C2"}, {12, "H1"}, {13, "H1"}, {14, "H1"}, {15, "H1"}, {16, "C2"}, {17, "CS"}, {18, "HC"}, {19, "HC"},
         {20, "CS"}, {21, "HC"}, {22, "HC"}, {23, "CS"}, {24, "HC"}, {25, "HC"}, {26, "CT"}, {27, "HC"}, {28, "HC"}, {29, "HC"},
         {30, "HC"}, {31, "HC"}, {32, "CS"}, {33, "HC"}, {34, "HC"}, {35, "CS"}, {36, "HC"}, {37, "HC"}, {38, "CS"}, {39, "HC"},
         {40, "HC"}, {41, "CT"}, {42, "HC"}, {43, "HC"}, {44, "HC"}, {45, "HC"}, {46, "HC"}, {47, "CS"}, {48, "HC"}, {49, "HC"},
         {50, "CS"}, {51, "HC"}, {52, "HC"}, {53, "CS"}, {54, "HC"}, {55, "HC"}, {56, "CT"}, {57, "HC"}, {58, "HC"}, {59, "HC"},
         {60, "HC"}, {61, "HC"}, {62, "CS"}, {63, "HC"}, {64, "HC"}, {65, "CS"}, {66, "HC"}, {67, "HC"}, {68, "CS"}, {69, "HC"},
         {70, "HC"}, {71, "CS"}, {72, "HC"}, {73, "HC"}, {74, "CS"}, {75, "HC"}, {76, "HC"}, {77, "CS"}, {78, "HC"}, {79, "HC"},
         {80, "CS"}, {81, "HC"}, {82, "HC"}, {83, "CS"}, {84, "HC"}, {85, "HC"}, {86, "CS"}, {87, "HC"}, {88, "HC"}, {89, "CS"},
         {90, "HC"}, {91, "HC"}, {92, "CS"}, {93, "HC"}, {94, "HC"}, {95, "CT"}, {96, "HC"}, {97, "HC"}, {98, "HC"}, {99, "HC"},
         {100, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), 1, 1.0e-6);
}
}; // namespace UnitTest

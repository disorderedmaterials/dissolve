// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "tests/testData.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(PCL2019AnionsAssignmentTest, beti)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Hydronium", "ImportXYZStructure", "xyz/beti.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 20);
    ASSERT_EQ(species.angles().size(), 37);
    ASSERT_EQ(species.torsions().size(), 42);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species,
                                    {{0, "C1F"}, {1, "CEF"}, {2, "F1"},  {3, "F1"},   {4, "SBT"},  {5, "NBT"},  {6, "OBT"},
                                     {7, "OBT"}, {8, "SBT"}, {9, "OBT"}, {10, "OBT"}, {11, "C1F"}, {12, "CEF"}, {13, "F1"},
                                     {14, "F1"}, {15, "F"},  {16, "F"},  {17, "F"},   {18, "F"},   {19, "F"},   {20, "F"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, BF4)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("BF4", "ImportXYZStructure", "xyz/bf4.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 4);
    ASSERT_EQ(species.angles().size(), 6);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "B"}, {1, "FB"}, {2, "FB"}, {3, "FB"}, {4, "FB"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c1SO3)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("C1SO3", "ImportXYZStructure", "xyz/c1so3.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 7);
    ASSERT_EQ(species.angles().size(), 12);
    ASSERT_EQ(species.torsions().size(), 9);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(
        &species, {{0, "SO"}, {1, "OS3"}, {2, "OS3"}, {3, "OS3"}, {4, "CS3"}, {5, "HS3"}, {6, "HS3"}, {7, "HS3"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c1SO4)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("C1SO4", "ImportXYZStructure", "xyz/c1so4.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 8);
    ASSERT_EQ(species.angles().size(), 13);
    ASSERT_EQ(species.torsions().size(), 6);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(
        &species, {{0, "HS4"}, {1, "CS4"}, {2, "OC4"}, {3, "HS4"}, {4, "HS4"}, {5, "SO"}, {6, "OS4"}, {7, "OS4"}, {8, "OS4"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c2SO3)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("C2SO3", "ImportXYZStructure", "xyz/c2so3.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 10);
    ASSERT_EQ(species.angles().size(), 18);
    ASSERT_EQ(species.torsions().size(), 18);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "SO"},
                                               {1, "OS3"},
                                               {2, "OS3"},
                                               {3, "OS3"},
                                               {4, "CS3"},
                                               {5, "CT3"},
                                               {6, "HS3"},
                                               {7, "HS3"},
                                               {8, "HC"},
                                               {9, "HC"},
                                               {10, "HC"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c2SO4)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("C2SO4", "ImportXYZStructure", "xyz/c2so4.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 11);
    ASSERT_EQ(species.angles().size(), 19);
    ASSERT_EQ(species.torsions().size(), 15);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "CT3"},
                                               {1, "CS4"},
                                               {2, "OC4"},
                                               {3, "HC"},
                                               {4, "HC"},
                                               {5, "HC"},
                                               {6, "HS4"},
                                               {7, "HS4"},
                                               {8, "SO"},
                                               {9, "OS4"},
                                               {10, "OS4"},
                                               {11, "OS4"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, c4fc1fsi)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("C4FC1FSI", "ImportXYZStructure", "xyz/c4fc1fsi.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 23);
    ASSERT_EQ(species.angles().size(), 43);
    ASSERT_EQ(species.torsions().size(), 51);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "CBT"},  {1, "F1"},  {2, "F1"},  {3, "F1"},   {4, "SBT"},  {5, "NBT"},
                                               {6, "OBT"},  {7, "OBT"}, {8, "SBT"}, {9, "OBT"},  {10, "OBT"}, {11, "C1F"},
                                               {12, "CSF"}, {13, "F1"}, {14, "F1"}, {15, "CSF"}, {16, "F"},   {17, "F"},
                                               {18, "CTF"}, {19, "F"},  {20, "F"},  {21, "F"},   {22, "F"},   {23, "F"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, CCN3)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("CCN3", "ImportXYZStructure", "xyz/ccn3.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 6);
    ASSERT_EQ(species.angles().size(), 6);
    ASSERT_EQ(species.torsions().size(), 6);
    ASSERT_EQ(species.impropers().size(), 1);

    systemTest.checkSpeciesAtomType(&species, {{0, "C3A"}, {1, "CN"}, {2, "CN"}, {3, "CN"}, {4, "NC"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, dca)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("DCA", "ImportXYZStructure", "xyz/dca.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 4);
    ASSERT_EQ(species.angles().size(), 3);
    ASSERT_EQ(species.torsions().size(), 2);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "N3A"}, {1, "CZA"}, {2, "NZA"}, {3, "CZA"}, {4, "NZA"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, fsi)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("FSI", "ImportXYZStructure", "xyz/fsi.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 8);
    ASSERT_EQ(species.angles().size(), 13);
    ASSERT_EQ(species.torsions().size(), 6);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(
        &species, {{0, "FSI"}, {1, "SBT"}, {2, "NBT"}, {3, "OBT"}, {4, "OBT"}, {5, "SBT"}, {6, "OBT"}, {7, "OBT"}, {8, "FSI"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, ntf2)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("NTf2", "ImportXYZStructure", "xyz/ntf2.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 14);
    ASSERT_EQ(species.angles().size(), 25);
    ASSERT_EQ(species.torsions().size(), 24);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "CBT"},
                                               {1, "F1"},
                                               {2, "F1"},
                                               {3, "F1"},
                                               {4, "SBT"},
                                               {5, "NBT"},
                                               {6, "OBT"},
                                               {7, "OBT"},
                                               {8, "SBT"},
                                               {9, "OBT"},
                                               {10, "OBT"},
                                               {11, "CBT"},
                                               {12, "F1"},
                                               {13, "F1"},
                                               {14, "F1"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, oac)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("OAc", "ImportXYZStructure", "xyz/oac.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 6);
    ASSERT_EQ(species.angles().size(), 9);
    ASSERT_EQ(species.torsions().size(), 6);
    ASSERT_EQ(species.impropers().size(), 1);

    systemTest.checkSpeciesAtomType(&species, {{0, "CTA"}, {1, "HC"}, {2, "HC"}, {3, "HC"}, {4, "CO2"}, {5, "O2"}, {6, "O2"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, otf)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("OTf", "ImportXYZStructure", "xyz/otf.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 7);
    ASSERT_EQ(species.angles().size(), 12);
    ASSERT_EQ(species.torsions().size(), 9);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(
        &species, {{0, "CBT"}, {1, "F1"}, {2, "F1"}, {3, "F1"}, {4, "SBT"}, {5, "OTF"}, {6, "OTF"}, {7, "OTF"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, PF6)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("PF6", "ImportXYZStructure", "xyz/pf6.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 6);
    ASSERT_EQ(species.angles().size(), 15);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "P"}, {1, "FP"}, {2, "FP"}, {3, "FP"}, {4, "FP"}, {5, "FP"}, {6, "FP"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, SCN)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("SCN", "ImportXYZStructure", "xyz/scn.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 2);
    ASSERT_EQ(species.angles().size(), 1);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species, {{0, "SK"}, {1, "CK"}, {2, "NK"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, tfa)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("TFA", "ImportXYZStructure", "xyz/tfa.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 6);
    ASSERT_EQ(species.angles().size(), 9);
    ASSERT_EQ(species.torsions().size(), 6);
    ASSERT_EQ(species.impropers().size(), 0);

    systemTest.checkSpeciesAtomType(&species,
                                    {{0, "CFA"}, {1, "FFA"}, {2, "FFA"}, {3, "FFA"}, {4, "CO2"}, {5, "O2F"}, {6, "O2F"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}

TEST(PCL2019AnionsAssignmentTest, tso)
{
    DissolveSystemTest systemTest;
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("TSO", "ImportXYZStructure", "xyz/tso.xyz",
                                                                          ForcefieldLibrary::forcefield("PCL2019/Anions"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 18);
    ASSERT_EQ(species.angles().size(), 30);
    ASSERT_EQ(species.torsions().size(), 36);
    ASSERT_EQ(species.impropers().size(), 6);

    systemTest.checkSpeciesAtomType(&species, {{0, "CATS"},
                                               {1, "CAOS"},
                                               {2, "CAMS"},
                                               {3, "CAOS"},
                                               {4, "CAMS"},
                                               {5, "CAPS"},
                                               {6, "CTTS"},
                                               {7, "SO"},
                                               {8, "HATS"},
                                               {9, "HATS"},
                                               {10, "HATS"},
                                               {11, "HATS"},
                                               {12, "HTS"},
                                               {13, "HTS"},
                                               {14, "HTS"},
                                               {15, "OS3"},
                                               {16, "OS3"},
                                               {17, "OS3"}});
    EXPECT_NEAR(species.totalCharge(true), -1, 1.0e-6);
}
}; // namespace UnitTest

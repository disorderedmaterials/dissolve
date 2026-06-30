// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(UFF4MOFMOF5AssignmentTest, MOF5)
{
    TestGraph testGraph;
    auto *importNode = testGraph.createNode("ImportXYZStructure");
    ASSERT_TRUE(importNode);
    ASSERT_TRUE(importNode->setOption<std::string>("FilePath", "xyz/mof5.xyz"));
    auto *setBox = testGraph.createNode("SetBox");
    ASSERT_TRUE(setBox);
    ASSERT_TRUE(setBox->setOption("Lengths", Vector3(25.8320, 25.8320, 25.8320)));
    auto *calculateBondingNode = testGraph.createNode("CalculateBonding");
    EXPECT_TRUE(calculateBondingNode);
    auto *speciesNode = dynamic_cast<SpeciesNode *>(testGraph.createNode("Species", "MOF5"));
    ASSERT_TRUE(speciesNode);
    auto *ffNode = testGraph.createNode("Forcefield");
    ASSERT_TRUE(ffNode);
    ASSERT_TRUE(ffNode->setOption("Forcefield", ForcefieldLibrary::forcefield("UFF4MOF").get()));
    ASSERT_TRUE(testGraph.addEdge({"ImportXYZStructure", "Structure", "SetBox", "Input"}));
    ASSERT_TRUE(testGraph.addEdge({"SetBox", "Output", "CalculateBonding", "Structure"}));
    ASSERT_TRUE(testGraph.addEdge({"CalculateBonding", "Structure", "MOF5", "Structure"}));
    ASSERT_TRUE(testGraph.addEdge({"Forcefield", "Recipe", "MOF5", "Recipe"}));

    // Run from the species node
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(speciesNode->versionIndex(), 0);

    auto &species = speciesNode->species();
    ASSERT_EQ(species.bonds().size(), 512);
    ASSERT_EQ(species.angles().size(), 912);
    ASSERT_EQ(species.torsions().size(), 1536);
    ASSERT_EQ(species.impropers().size(), 192);

    EXPECT_TRUE(testSpeciesAtomType(&species, {{0, "Zn3f2"},
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
                                               {386, "H_"}}));
    testSpeciesIntramolecular(&species, {386, 290}, {BondFunctions::Form::Harmonic, "k=2991.0611 eq=1.08142"}, 1.0e-5);
    testSpeciesIntramolecular(&species, {0, 32}, {BondFunctions::Form::Harmonic, "k=1337.872 eq=1.84185"}, 3.0e-5);
    testSpeciesIntramolecular(&species, {8, 32}, {BondFunctions::Form::Harmonic, "k=1337.872 eq=1.84185"}, 1.0e-5);
    testSpeciesIntramolecular(&species, {71, 156}, {BondFunctions::Form::Harmonic, "k=5048.4039 eq=1.34262"}, 5.0e-6);
    testSpeciesIntramolecular(&species, {178, 122, 26}, {AngleFunctions::Form::Cosine, "53.5872 3 0 -1"}, 3.0e-5);
    testSpeciesIntramolecular(&species, {121, 29, 112}, {AngleFunctions::Form::Cosine, "107.58286 3 0 -1"}, 6.0e-6);
    testSpeciesIntramolecular(&species, {178, 122, 26, 117}, {TorsionFunctions::Form::UFFCosine, "96.4708  2  180"}, 5.0e-5);
    testSpeciesIntramolecular(&species, {178, 122, 133, 226}, {TorsionFunctions::Form::FourierN, "25.104 1 -1 0"});
}
}; // namespace UnitTest

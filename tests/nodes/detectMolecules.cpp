// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/detectMolecules.h"
#include "classes/configuration.h"
#include "nodes/calculateBonding.h"
#include "nodes/importXYZStructure.h"
#include "nodes/neutronSQ.h"
#include "tests/testGraph.h"
#include <gtest/gtest.h>
#include <string>
#include "nodes/iterableGraph.h"
#include "nodes/gr.h"
#include "nodes/sq.h"

namespace UnitTest
{
testing::AssertionResult compareContents(const Structure &structure, const Configuration *configuration, bool fold = true)
{
    for (const auto &structureAtom : structure.atoms())
    {
        auto r = fold ? configuration->box().fold(structureAtom->r()) : structureAtom->r();
        if (std::ranges::none_of(configuration->atoms(),
                                 [&structureAtom, r](const auto &cfgAtom)
                                 {
                                     auto isSameAtom = structureAtom->Z() == cfgAtom.Z();
                                     auto isCloseX = fabs(r.x - cfgAtom.r().x) < 1.0e-6;
                                     auto isCloseY = fabs(r.y - cfgAtom.r().y) < 1.0e-6;
                                     auto isCloseZ = fabs(r.z - cfgAtom.r().z) < 1.0e-6;
                                     return isSameAtom && isCloseX && isCloseY && isCloseZ;
                                 }))
            return testing::AssertionFailure()
                   << std::format("Failed to find atom {} @ {},{},{} in the reconstructed structure.",
                                  Elements::symbol(structureAtom->Z()), r.x, r.y, r.z);
    }

    return testing::AssertionSuccess();
}

TEST(DetectMoleculesNodeTest, Water33Unordered)
{
    TestGraph testGraph;

    // Load the xyz file - a system of 33 water molecules in a 10x10x10 Angstrom cubic box, atoms randomly ordered
    auto importXYZStructureNode = testGraph.appendNode("ImportXYZStructure");
    ASSERT_TRUE(importXYZStructureNode);
    importXYZStructureNode->setOption("FilePath",
                                      std::filesystem::path("/home/tris/src/dissolve/build-release/bin/10_298.xyz"));

    // Set the periodic box in the structure
    auto setBoxNode = testGraph.appendNode("SetBox");
    ASSERT_TRUE(setBoxNode);
    setBoxNode->setOption("Lengths", Vector3(105.566, 91.431, 75.996));
    ASSERT_TRUE(testGraph.addEdge({"ImportXYZStructure", "Structure", "SetBox", "Input"}));

    // Calculate bonding
    auto cb = testGraph.appendNode("CalculateBonding");
    cb->setOption("Tolerance", Number(1.2));
    ASSERT_TRUE(testGraph.addEdge({"SetBox", "Output", "CalculateBonding", "Structure"}));

    // Append DetectMolecules
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.appendNode("DetectMolecules"));
    ASSERT_TRUE(detectMoleculesNode);
    ASSERT_TRUE(testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"}));

    // Run to get detected structures
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().size(), 2);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().at("Cl").instances().size(), 240);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().at("O9C111H195").instances().size(), 240);

    // Create a species from the detected structure
    auto clNode = testGraph.appendNode("Species", "Cl");
    ASSERT_TRUE(clNode);
    ASSERT_TRUE(testGraph.addEdge({"DetectMolecules", "Cl", "Cl", "Structure"}));
    auto molNode = testGraph.appendNode("Species", "Mol");
    ASSERT_TRUE(molNode);
    ASSERT_TRUE(testGraph.addEdge({"DetectMolecules", "O9C111H195", "Mol", "Structure"}));

    // Create a configuration
    ASSERT_TRUE(testGraph.appendNode("Configuration"));
    auto instantiateNode1 = testGraph.appendNode("Instantiate", "I1");
    ASSERT_TRUE(instantiateNode1);
    ASSERT_TRUE(testGraph.addEdge({"Configuration", "Configuration", "I1", "Configuration"}));
    ASSERT_TRUE(testGraph.addEdge({"Cl", "Species", "I1", "Species"}));
    auto instantiateNode2 = testGraph.appendNode("Instantiate", "I2");
    ASSERT_TRUE(instantiateNode2);
    ASSERT_TRUE(testGraph.addEdge({"I1", "Configuration", "I2", "Configuration"}));
    ASSERT_TRUE(testGraph.addEdge({"Mol", "Species", "I2", "Species"}));

    // // Run from the instantiate node
    // ASSERT_EQ(instantiateNode2->run(), NodeConstants::ProcessResult::Success);
    //
    // // Check consistency between the original XYZ structure and the reconstructed configuration
    // ASSERT_TRUE(compareContents(importXYZStructureNode->getOutputValue<Structure>("Structure"),
    //                             instantiateNode2->getOutputValue<Configuration *>("Configuration")));

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "/home/tris/src/dissolve/build-release/bin/10_298.xyz");
    EXPECT_TRUE(iterator);
    ASSERT_TRUE(iterator->setOption<Number>("N", 10));

    auto &&[gr, sq] = testGraph.appendGRSQ();
    ASSERT_TRUE(sq->setOption<Number>("QDelta", 0.01));
    ASSERT_TRUE(gr->setOption<Number>("BinWidth", 0.03));
    testGraph.appendNeutronSQ(sq, "Test");

    iterator->run();
}

TEST(DetectMoleculesNodeTest, WaterTEST)
{
    TestGraph testGraph;

    // Load the xyz file - a system of 33 water molecules in a 10x10x10 Angstrom cubic box, atoms randomly ordered
    auto importXYZStructureNode = testGraph.appendNode("ImportXYZStructure");
    ASSERT_TRUE(importXYZStructureNode);
    importXYZStructureNode->setOption("FilePath",
    std::filesystem::path("dlpoly/water267-analysis/water-267-298K.xyz"));

    // Set the periodic box in the structure
    auto setBoxNode = testGraph.appendNode("SetBox");
    ASSERT_TRUE(setBoxNode);
    setBoxNode->setOption("Lengths", Vector3(20.0,20.0,20.0));
    ASSERT_TRUE(testGraph.addEdge({"ImportXYZStructure", "Structure", "SetBox", "Input"}));

    // Calculate bonding
    auto cb = testGraph.appendNode("CalculateBonding");
    cb->setOption("Tolerance", Number(1.2));
    ASSERT_TRUE(testGraph.addEdge({"SetBox", "Output", "CalculateBonding", "Structure"}));

    // Append DetectMolecules
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.appendNode("DetectMolecules"));
    ASSERT_TRUE(detectMoleculesNode);
    ASSERT_TRUE(testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"}));

    // Run to get detected structures
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().size(), 1);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().at("OH2").instances().size(), 267);

    // Create a species from the detected structure
    auto clNode = testGraph.appendNode("Species", "Water");
    ASSERT_TRUE(clNode);
    ASSERT_TRUE(testGraph.addEdge({"DetectMolecules", "OH2", "Water", "Structure"}));

    // Create a configuration
    ASSERT_TRUE(testGraph.appendNode("Configuration"));
    auto instantiateNode1 = testGraph.appendNode("Instantiate", "I1");
    ASSERT_TRUE(instantiateNode1);
    ASSERT_TRUE(testGraph.addEdge({"Configuration", "Configuration", "I1", "Configuration"}));
    ASSERT_TRUE(testGraph.addEdge({"Water", "Species", "I1", "Species"}));

    // // Run from the instantiate node
    // ASSERT_EQ(instantiateNode2->run(), NodeConstants::ProcessResult::Success);
    //
    // // Check consistency between the original XYZ structure and the reconstructed configuration
    // ASSERT_TRUE(compareContents(importXYZStructureNode->getOutputValue<Structure>("Structure"),
    //                             instantiateNode2->getOutputValue<Configuration *>("Configuration")));

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
    EXPECT_TRUE(iterator);
    ASSERT_TRUE(iterator->setOption<Number>("N", 90));

    auto &&[gr, sq] = testGraph.appendGRSQ();
    ASSERT_TRUE(gr->setOption<Number>("BinWidth", 0.03));
    testGraph.appendNeutronSQ(sq, "Test");

    iterator->run();
}
} // namespace UnitTest

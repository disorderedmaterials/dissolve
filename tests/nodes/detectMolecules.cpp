// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/detectMolecules.h"
#include "nodes/calculateBonding.h"
#include "nodes/importXYZStructure.h"
#include "tests/testGraph.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{
TEST(DetectMoleculesNodeTest, Water33Unordered)
{
    TestGraph testGraph;

    // Load the xyz file - a system of 33 water molecules in a 10x10x10 Angstrom cubic box, atoms randomly ordered
    auto importXYZStructureNode = testGraph.appendNode("ImportXYZStructure");
    ASSERT_TRUE(importXYZStructureNode);
    importXYZStructureNode->setOption("FilePath", std::string("xyz/water33unordered.xyz"));

    // Set the periodic box in the structure
    auto setBoxNode = testGraph.appendNode("SetBox");
    ASSERT_TRUE(setBoxNode);
    setBoxNode->setOption("Lengths", Vector3(10.0, 10.0, 10.0));
    ASSERT_TRUE(testGraph.addEdge({"ImportXYZStructure", "Structure", "SetBox", "Input"}));

    // Calculate bonding
    EXPECT_TRUE(testGraph.appendNode("CalculateBonding"));
    ASSERT_TRUE(testGraph.addEdge({"SetBox", "Output", "CalculateBonding", "Structure"}));

    // Append DetectMolecules
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.appendNode("DetectMolecules"));
    ASSERT_TRUE(detectMoleculesNode);
    ASSERT_TRUE(testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"}));

    // Run to get detected structures
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().size(), 1);
    ASSERT_EQ(detectMoleculesNode->detectedStructures().at(0).instances().size(), 33);

    // Create a configuration
    ASSERT_TRUE(testGraph.appendNode("Configuration"));
    auto setBoxConfigurationNode = testGraph.appendNode("SetBox", "SetBoxConfiguration");
    ASSERT_TRUE(setBoxConfigurationNode);
    setBoxConfigurationNode->setOption("Lengths", Vector3(10.0, 10.0, 10.0));
    ASSERT_TRUE(testGraph.addEdge({"Configuration", "Configuration", "SetBoxConfiguration", "Input"}));

    // Create a species from the detected structure
    auto speciesNode = testGraph.appendNode("Species", "Water");
    ASSERT_TRUE(speciesNode);
    ASSERT_TRUE(testGraph.addEdge({"DetectMolecules", "DetectedMolecule-0", "Water", "Structure"}));

    auto insertNode = testGraph.appendNode("Insert");
    ASSERT_TRUE(insertNode);
    ASSERT_TRUE(insertNode->setOption("BoxAction", InsertNode::BoxActionStyle::None));
    ASSERT_TRUE(testGraph.addEdge({"SetBoxConfiguration", "Output", "Insert", "Configuration"}));
    ASSERT_TRUE(testGraph.addEdge({"Water", "Species", "Insert", "Species"}));
    ASSERT_TRUE(testGraph.addEdge({"DetectMolecules", "DetectedMolecule-0", "Insert", "Instances"}));

    ASSERT_EQ(insertNode->run(), NodeConstants::ProcessResult::Success);
}
} // namespace UnitTest
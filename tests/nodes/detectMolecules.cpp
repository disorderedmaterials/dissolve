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
    auto importXYZStructure = testGraph.appendNode("ImportXYZStructure");
    ASSERT_TRUE(importXYZStructure);
    importXYZStructure->setOption("FilePath", std::string("xyz/water33unordered.xyz"));

    // Set the periodic box in the structure
    auto setBox = testGraph.appendNode("SetBox");
    ASSERT_TRUE(setBox);
    setBox->setOption("Lengths", Vector3(10.0, 10.0, 10.0));
    ASSERT_TRUE(testGraph.addEdge({"ImportXYZStructure", "Structure", "SetBox", "Input"}));

    // Calculate bonding
    EXPECT_TRUE(testGraph.appendNode("CalculateBonding"));
    ASSERT_TRUE(testGraph.addEdge({"SetBox", "Output", "CalculateBonding", "Structure"}));

    // Append DetectMolecules
    auto detectMolecules = static_cast<DetectMoleculesNode *>(testGraph.appendNode("DetectMolecules"));
    ASSERT_TRUE(detectMolecules);
    ASSERT_TRUE(testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"}));

    // Run to get detected structures
    ASSERT_EQ(detectMolecules->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(detectMolecules->detectedStructures().size(), 1);
    ASSERT_EQ(detectMolecules->detectedStructures().at(0).instances().size(), 33);
}
} // namespace UnitTest
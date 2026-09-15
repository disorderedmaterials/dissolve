// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/detectMolecules.h"
#include "classes/configuration.h"
#include "nodes/calculateBonding.h"
#include "nodes/importXYZStructure.h"
#include "tests/testGraph.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{
testing::AssertionResult compareContents(const Structure &structure, const Configuration *configuration, bool fold = true)
{
    for (const auto &structureAtom : structure.atoms())
    {
        auto r = fold ? configuration->box().fold(structureAtom->r()) : structureAtom->r();
        if (std::ranges::find_if(configuration->atoms(),
                                 [&structureAtom, r](const auto &cfgAtom)
                                 {
                                     return structureAtom->Z() == cfgAtom.Z() && fabs(r.x - cfgAtom.r().x) < 1.0e-6 &&
                                            fabs(r.y - cfgAtom.r().y) < 1.0e-6 && fabs(r.z - cfgAtom.r().z) < 1.0e-6;
                                 }) == configuration->atoms().end())
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
    ASSERT_EQ(detectMoleculesNode->detectedStructures().at("OH2").instances().size(), 33);

    // Create a species from the detected structure
    auto speciesNode = testGraph.appendNode("Species", "Water");
    ASSERT_TRUE(speciesNode);
    ASSERT_TRUE(testGraph.addEdge({"DetectMolecules", "OH2", "Water", "Structure"}));

    // Create a configuration
    ASSERT_TRUE(testGraph.appendNode("Configuration"));
    auto instantiateNode = testGraph.appendNode("Instantiate");
    ASSERT_TRUE(instantiateNode);
    ASSERT_TRUE(testGraph.addEdge({"Configuration", "Configuration", "Instantiate", "Configuration"}));
    ASSERT_TRUE(testGraph.addEdge({"Water", "Species", "Instantiate", "Species"}));

    // Run from the instantiate node
    ASSERT_EQ(instantiateNode->run(), NodeConstants::ProcessResult::Success);

    // Check consistency between the original XYZ structure and the reconstructed configuration
    ASSERT_TRUE(compareContents(importXYZStructureNode->getOutputValue<Structure>("Structure"),
                                instantiateNode->getOutputValue<Configuration *>("Configuration")));
}
} // namespace UnitTest

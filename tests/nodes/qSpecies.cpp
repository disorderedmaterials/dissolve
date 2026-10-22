// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/analysis/qSpecies.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(QSpeciesNodeTest, Simple)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration(
        "Box", {{testGraph.createAtomicSpecies(Elements::Si), 10}, {testGraph.createAtomicSpecies(Elements::O), 11}},
        {30.0, 30.0, 30.0}));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/qSpecies-simple.xyz"));

    // Grab atomic species and set up simple sites
    auto &speciesSi = *testGraph.findSpecies("Si");
    auto &speciesO = *testGraph.findSpecies("O");

    // Add a QSpecies node to the iterator
    auto qSpecies = dynamic_cast<QSpeciesNode *>(testGraph.createNode("QSpecies"));
    ASSERT_TRUE(qSpecies);
    ASSERT_TRUE(qSpecies->setOption<SpeciesSites>("NetworkFormer", {{speciesSi.findSite("Origin")}}));
    ASSERT_TRUE(qSpecies->setOption<SpeciesSites>("BridgingOxygen", {{speciesO.findSite("Origin")}}));
    ASSERT_TRUE(qSpecies->setOption<Range>("DistanceRange", {0.0, 2.1}));
    EXPECT_TRUE(testGraph.addEdge({testGraph.fetchHeadName(), "Configuration", "QSpecies", "Configuration"}));

    // Run from the qSpecies node
    ASSERT_EQ(qSpecies->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(qSpecies->versionIndex(), 0);

    // QSpecies types and fractional populations
    std::vector<std::pair<int, double>> referenceData = {{0, 0.2}, {1, 0.3}, {2, 0.3}, {3, 0.1}, {4, 0.1}};

    auto data = qSpecies->qSpecies();
    for (auto &[qType, fraction] : referenceData)
        EXPECT_DOUBLE_EQ(data.value(qType), fraction);
}

} // namespace UnitTest
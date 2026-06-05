// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/modifierOSites.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ModifierOSitesNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"Si", 9}, {"O", 13}, {"P", 8}}, {30.0, 30.0, 30.0}));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/modifierOSites.xyz"));

    // Grab species pointers
    auto *Si = testGraph.findNode("Si")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(Si);
    auto *O = testGraph.findNode("O")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(O);
    auto *P = testGraph.findNode("P")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(P);

    // Add the analysis module to the iterator
    auto modOS = dynamic_cast<ModifierOSitesNode *>(testGraph.createNode("ModifierOSites"));
    ASSERT_TRUE(modOS);
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("Modifier", {{P->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("BondingOxygen", {{O->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("NetworkFormer", {{Si->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<Range>("DistanceRange", {0.0, 2.1}));
    ASSERT_TRUE(modOS->setOption<Range>("ModifierDistanceRange", {0.0, 2.1}));
    ASSERT_TRUE(testGraph.addEdge({testGraph.fetchHeadName(), "Configuration", "ModifierOSites", "Configuration"}));

    // Run from the modifier O sites node explicitly
    ASSERT_EQ(modOS->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(modOS->versionIndex(), 0);

    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        modOS->rdfBC(), "B-C RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", Data1DImportFileFormat::Data1DImportFormat::XY},
        4.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(modOS->angleABC(), "A-B-C angle",
                                                {"dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm",
                                                 Data1DImportFileFormat::Data1DImportFormat::XY},
                                                3.0e-6));
}

} // namespace UnitTest
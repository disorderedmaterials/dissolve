// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/units.h"
#include "classes/isotopologueSet.h"
#include "data/structureFactors.h"
#include "io/import/data1D.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(GraphArgonTest, InitSimulation)
{
    GraphTestData data;
    createArgonGraph(&data.graphRoot);

    // Get the Insert node and run the graph
    auto insertNode = data.graphRoot.findNode("Insert");
    ASSERT_TRUE(insertNode);
    ASSERT_EQ(insertNode->run(), NodeConstants::ProcessResult::Success);

    // Check Configuration contents
    auto *cfg = insertNode->getOutputValue<Configuration *>("Configuration");
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nMolecules(), insertNode->getInputValue<Number>("Population").asInteger());
};

TEST(GraphArgonTest, AllCorrelations)
{
    GraphTestData data;
    createArgonGraph(&data.graphRoot);

    // Add in imported coordinates
    auto importConfigCoordsNode = data.graphRoot.createNode("ImportConfigurationCoordinates", "BulkXYZ");
    ASSERT_TRUE(importConfigCoordsNode);
    ASSERT_TRUE(importConfigCoordsNode->setOption<std::string>("FilePath", "dissolve2/argon/Ar_bulk_step1000.xyz"));
    ASSERT_TRUE(importConfigCoordsNode->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
        "FileFormat", CoordinateImportFileFormat::CoordinateImportFormat::XYZ));
    ASSERT_TRUE(data.graphRoot.addEdge({"Insert", "Configuration", "BulkXYZ", "Configuration"}));

    // Add GR node and link in configuration
    auto grNode = data.graphRoot.createNode("GR", "GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.025));
    ASSERT_TRUE(data.graphRoot.addEdge({"BulkXYZ", "Configuration", "GR", "Configuration"}));

    // Add SQ node and link in GR
    auto sqNode = data.graphRoot.createNode("SQ", "SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(data.graphRoot.addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    // Add NeutronSQ node and link in data
    auto neutronSQNode = data.graphRoot.createNode("NeutronSQ", "NeutronSQ");
    ASSERT_TRUE(neutronSQNode);
    ASSERT_TRUE(neutronSQNode->setOption<StructureFactors::NormalisationType>("ReferenceNormalisedTo",
                                                                              StructureFactors::SquareOfAverageNormalisation));
    IsotopologueSet isotopologues;
    auto arNode = dynamic_cast<SpeciesNode *>(data.graphRoot.findNode("Argon"));
    ASSERT_TRUE(arNode);
    isotopologues.add(arNode->species().findIsotopologue("Ar36"), 1.0);
    ASSERT_TRUE(neutronSQNode->setOption<IsotopologueSet>("Isotopologues", isotopologues));
    ASSERT_TRUE(data.graphRoot.addEdge({"SQ", "UnweightedGR", "NeutronSQ", "UnweightedGR"}));
    ASSERT_TRUE(data.graphRoot.addEdge({"SQ", "UnweightedSQ", "NeutronSQ", "UnweightedSQ"}));

    // Set reference F(Q) data
    auto data1DImportNode = data.graphRoot.createNode("Data1DImport", "Yarnell");
    ASSERT_TRUE(data1DImportNode);
    ASSERT_TRUE(data1DImportNode->setOption<std::string>("FilePath", "dissolve2/argon/yarnell.sq"));
    ASSERT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
        "ImportFormat", Data1DImportFileFormat::Data1DImportFormat::XY));
    ASSERT_TRUE(data1DImportNode->setOption<std::optional<Number>>("RemoveAverageFromX", 9.0));
    ASSERT_TRUE(data.graphRoot.addEdge({"Yarnell", "Data", "NeutronSQ", "ReferenceData"}));

    /*
     * Run the Graph
     */

    ASSERT_EQ(neutronSQNode->run(), NodeConstants::ProcessResult::Success);

    // Check total unweighted SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    ASSERT_TRUE(unweightedSQ);
    ASSERT_TRUE(
        DissolveSystemTest::checkData1D(unweightedSQ->total(), "UnweightedSQ", {"dissolve2/argon/SQ01-UnweightedSQ-total.sq"}));

    // Check neutron weighted SQ
    const auto tolerance = 0.025;
    auto weightedSQ = neutronSQNode->getOutputValue<PartialSet *>("WeightedSQ");
    ASSERT_TRUE(weightedSQ);
    ASSERT_TRUE(DissolveSystemTest::checkData1D(weightedSQ->total(), "WeightedSQ",
                                                {"dissolve2/argon/NeutronSQ01-WeightedSQ-total.sq"}, tolerance));
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/siteRDF.h"
#include "classes/speciesSites.h"
#include "io/import/trajectory.h"
#include "math/rangedVector3.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class SiteRDFTest : public ::testing::Test
{
    public:
    SiteRDFTest() { setUp(); }
    ~SiteRDFTest() = default;

    protected:
    bool importTrajectory(std::string name = "dlpoly/water267-analysis/water-267-298K.xyz")
    {
        if (!(cfgTrajImporter() && iterator()))
            return false;

        cfgTrajImporter()->setOption<std::string>("FilePath", name);
        cfgTrajImporter()->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>(
            "FileFormat", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

        return iterator()->addEdge({"Inputs", "Configuration", "ImportConfigurationTrajectory", "Configuration"});
    }
    Node *cfgTrajImporter() { return iterator() ? iterator()->findNode("ImportConfigurationTrajectory") : nullptr; }
    Node *siteRDF() { return iterator() ? iterator()->findNode("SiteRDF") : nullptr; }
    IterableGraph *iterator() { return static_cast<IterableGraph *>(root()->findNode("Iterator")); }
    const Species *water() { return root()->findNode("Water")->getOutputValue<const Species *>("Species"); }
    DissolveGraph *root() { return &(testData_.graphRoot); }
    void setUp()
    {
        // Add iterator
        ASSERT_TRUE(dynamic_cast<IterableGraph *>(root()->createNode("Iterator")));

        // Create water graph
        createWaterGraph(root(), 267);

        // Create a dynamic input from the graph's existing Insert node
        EXPECT_TRUE(root()->addEdge({"Insert", "Configuration", "Iterator", "Configuration"}));

        // Create an import configuration trajectory node
        ASSERT_TRUE(iterator()->createNode("ImportConfigurationTrajectory"));

        // Import water trajectory
        EXPECT_TRUE(importTrajectory());

        // Create a site RDF node
        ASSERT_TRUE(iterator()->createNode("SiteRDF"));
    }

    // Root test data
    GraphTestData testData_;
};

TEST_F(SiteRDFTest, Water)
{
    // Set options
    ASSERT_TRUE(siteRDF()->setOption<SpeciesSites>("SiteA", {{water()->findSite("COM")}}));
    ASSERT_TRUE(siteRDF()->setOption<SpeciesSites>("SiteB", {{water()->findSite("COM")}}));
    ASSERT_TRUE(siteRDF()->setOption<Vector3>("DistanceRange", {0.0, 20.0, 0.05}));
    ASSERT_TRUE(siteRDF()->setOption<Range>("RangeA", {0.0, 3.3}));
    ASSERT_TRUE(siteRDF()->setOption<Range>("RangeB", {3.3, 5.6}));
    ASSERT_TRUE(siteRDF()->setOption("RangeBEnabled", true));
    ASSERT_TRUE(iterator()->addEdge({"ImportConfigurationTrajectory", "Configuration", "SiteRDF", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator()->setOption<Number>("N", 95));
    auto result = iterator()->run();
    ASSERT_EQ(result, NodeConstants::ProcessResult::Success);
}

} // namespace UnitTest
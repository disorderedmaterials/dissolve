// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/analysis/sdf.h"
#include "nodes/import/dlputilsPDens.h"
#include "nodes/iterableGraph.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(SDFNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{testGraph.importSpecies("species/water-dlpoly.toml"), 267}}, 0.1);

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis module to the iterator
    auto sdf = dynamic_cast<SDFNode *>(iterator->createNode("SDF"));
    ASSERT_TRUE(sdf);
    auto *water = testGraph.findSpecies("Water");
    ASSERT_TRUE(water);
    ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteA", {{water->findSite("HMidpoint")}}));
    ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteB", {{water->findSite("HMidpoint")}}));
    ASSERT_TRUE(sdf->setOption<Vector3>("RangeX", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(sdf->setOption<Vector3>("RangeY", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(sdf->setOption<Vector3>("RangeZ", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "SDF", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    Data3D referenceData;
    EXPECT_TRUE(ImportDLPUtilsPDensNode::read(referenceData, "dlpoly/water267-analysis/water-267-298K.11.pdens"));
    EXPECT_TRUE(testData3D(sdf->sdf(), "SDF", referenceData, "dlpoly/water267-analysis/water-267-298K.11.pdens", 0.13));
}

TEST(SDFNodeTest, Benzene)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{testGraph.importSpecies("species/benzene.toml"), 181}},
                                  {29.925089931000, 29.925089931000, 29.925089931000});

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/benzene181/benzene181.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis module to the iterator
    auto sdf = dynamic_cast<SDFNode *>(iterator->createNode("SDF"));
    ASSERT_TRUE(sdf);
    auto *benzene = testGraph.findSpecies("Benzene");
    ASSERT_TRUE(benzene);
    ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteA", {{benzene->findSite("Ring")}}));
    ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteB", {{benzene->findSite("Ring")}}));
    ASSERT_TRUE(sdf->setOption<Vector3>("RangeX", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(sdf->setOption<Vector3>("RangeY", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(sdf->setOption<Vector3>("RangeZ", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "SDF", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 80));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    Data3D referenceData;
    EXPECT_TRUE(ImportDLPUtilsPDensNode::read(referenceData, "dlpoly/benzene181/benzene181.11.pdens"));
    EXPECT_TRUE(testData3D(sdf->sdf(), "SDF", referenceData, "dlpoly/benzene181/benzene181.11.pdens", 0.3));
}
} // namespace UnitTest
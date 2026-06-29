// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/sdf.h"
#include "nodes/importDLPUtilsPDens.h"
#include "nodes/iterableGraph.h"
#include "nodes/species.h"
#include "tests/testGraphFixture.h"

namespace UnitTest
{
class SDFNodeWaterTest : public TestGraphFixture
{
    private:
    Data3D referenceData_;

    protected:
    // Prepare any necessary test data
    void prepareTestData() override
    {
        ASSERT_TRUE(ImportDLPUtilsPDensNode::read(referenceData_, "dlpoly/water267-analysis/water-267-298K.11.pdens"));
    }
    // Perform graph construction
    void constructGraph() override
    {
        testGraph_.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, 0.1);

        // Create trajectory iterator
        auto iterator =
            testGraph_.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
        ASSERT_TRUE(iterator);
        ASSERT_TRUE(iterator->setOption<Number>("N", 95));

        // Add the analysis module to the iterator
        auto sdf = dynamic_cast<SDFNode *>(iterator->createNode("SDF"));
        ASSERT_TRUE(sdf);
        auto *water = testGraph_.findNode("Water")->getOutputValue<const Species *>("Species");
        ASSERT_TRUE(water);
        ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteA", {{water->findSite("HMidpoint")}}));
        ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteB", {{water->findSite("HMidpoint")}}));
        ASSERT_TRUE(sdf->setOption<Vector3>("RangeX", {-10.25, 10.25, 0.5}));
        ASSERT_TRUE(sdf->setOption<Vector3>("RangeY", {-10.25, 10.25, 0.5}));
        ASSERT_TRUE(sdf->setOption<Vector3>("RangeZ", {-10.25, 10.25, 0.5}));
        ASSERT_TRUE(iterator->addEdge({testGraph_.fetchHeadName(), "Configuration", "SDF", "Configuration"}));
    }
    // Run the graph
    void runGraph()
    {
        auto *iterator = findNode<IterableGraph>("Iterator");
        ASSERT_TRUE(iterator);
        ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);
    }
    // Perform tests on generated data
    void performTests()
    {
        auto *iterator = findNode<IterableGraph>("Iterator");
        ASSERT_TRUE(iterator);
        auto *sdf = dynamic_cast<SDFNode *>(iterator->findNode("SDF"));
        ASSERT_TRUE(sdf);
        EXPECT_TRUE(testData3D(sdf->sdf(), "SDF", referenceData_, "dlpoly/water267-analysis/water-267-298K.11.pdens", 0.13));
    }
};

TEST_F(SDFNodeWaterTest, Water) { go(); }
// {
//     // Set up the test graph
//     TestGraph testGraph;
//     testGraph.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, 0.1);
//
//     // Create trajectory iterator
//     auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
//     EXPECT_TRUE(iterator);
//
//     // Add the analysis module to the iterator
//     auto sdf = dynamic_cast<SDFNode *>(iterator->createNode("SDF"));
//     ASSERT_TRUE(sdf);
//     auto *water = testGraph.findNode("Water")->getOutputValue<const Species *>("Species");
//     ASSERT_TRUE(water);
//     ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteA", {{water->findSite("HMidpoint")}}));
//     ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteB", {{water->findSite("HMidpoint")}}));
//     ASSERT_TRUE(sdf->setOption<Vector3>("RangeX", {-10.25, 10.25, 0.5}));
//     ASSERT_TRUE(sdf->setOption<Vector3>("RangeY", {-10.25, 10.25, 0.5}));
//     ASSERT_TRUE(sdf->setOption<Vector3>("RangeZ", {-10.25, 10.25, 0.5}));
//     ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "SDF", "Configuration"}));
//
//     // Run from the iterator node explicitly
//     ASSERT_TRUE(iterator->setOption<Number>("N", 95));
//     ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);
//
//     Data3D referenceData;
//     EXPECT_TRUE(ImportDLPUtilsPDensNode::read(referenceData, "dlpoly/water267-analysis/water-267-298K.11.pdens"));
//     EXPECT_TRUE(testData3D(sdf->sdf(), "SDF", referenceData, "dlpoly/water267-analysis/water-267-298K.11.pdens", 0.13));
// }
//
// TEST(SDFNodeTest, Benzene)
// {
//     // Set up the test graph
//     TestGraph testGraph;
//     testGraph.createConfiguration("Box", {{"species/benzene.toml", 181}},
//     {29.925089931000, 29.925089931000, 29.925089931000});
//
//     // Create trajectory iterator
//     auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/benzene181/benzene181.xyz");
//     EXPECT_TRUE(iterator);
//
//     // Add the analysis module to the iterator
//     auto sdf = dynamic_cast<SDFNode *>(iterator->createNode("SDF"));
//     ASSERT_TRUE(sdf);
//     auto *benzene = testGraph.findNode("Benzene")->getOutputValue<const Species *>("Species");
//     ASSERT_TRUE(benzene);
//     ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteA", {{benzene->findSite("Ring")}}));
//     ASSERT_TRUE(sdf->setOption<SpeciesSites>("SiteB", {{benzene->findSite("Ring")}}));
//     ASSERT_TRUE(sdf->setOption<Vector3>("RangeX", {-10.25, 10.25, 0.5}));
//     ASSERT_TRUE(sdf->setOption<Vector3>("RangeY", {-10.25, 10.25, 0.5}));
//     ASSERT_TRUE(sdf->setOption<Vector3>("RangeZ", {-10.25, 10.25, 0.5}));
//     ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "SDF", "Configuration"}));
//
//     // Run from the iterator node explicitly
//     ASSERT_TRUE(iterator->setOption<Number>("N", 80));
//     ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);
//
//     Data3D referenceData;
//     EXPECT_TRUE(ImportDLPUtilsPDensNode::read(referenceData, "dlpoly/benzene181/benzene181.11.pdens"));
//     EXPECT_TRUE(testData3D(sdf->sdf(), "SDF", referenceData, "dlpoly/benzene181/benzene181.11.pdens", 0.3));
// }
} // namespace UnitTest
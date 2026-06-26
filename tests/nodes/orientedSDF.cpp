// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/orientedSDF.h"
#include "nodes/importDLPUtilsPDens.h"
#include "nodes/iterableGraph.h"
#include "nodes/species.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(OrientedSDFNodeTest, Benzene)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"species/benzene.toml", 181}}, {29.925089931000, 29.925089931000, 29.925089931000});

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/benzene181/benzene181.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis module to the iterator
    auto osdf = dynamic_cast<OrientedSDFNode *>(iterator->createNode("OrientedSDF"));
    ASSERT_TRUE(osdf);
    auto *benzene = testGraph.findNode("Benzene")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(benzene);
    ASSERT_TRUE(osdf->setOption<SpeciesSites>("SiteA", {{benzene->findSite("Ring")}}));
    ASSERT_TRUE(osdf->setOption<Site::SiteAxis>("AxisA", Site::SiteAxis::ZAxis));
    ASSERT_TRUE(osdf->setOption<SpeciesSites>("SiteB", {{benzene->findSite("Ring")}}));
    ASSERT_TRUE(osdf->setOption<Site::SiteAxis>("AxisB", Site::SiteAxis::ZAxis));
    ASSERT_TRUE(osdf->setOption<Vector3>("RangeX", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(osdf->setOption<Vector3>("RangeY", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(osdf->setOption<Vector3>("RangeZ", {-10.25, 10.25, 0.5}));
    ASSERT_TRUE(osdf->setOption<Range>("AngleRange", {-10.0, 10.0}));
    ASSERT_TRUE(osdf->setOption<bool>("Symmetric", true));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "OrientedSDF", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 80));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    Data3D referenceData;
    EXPECT_TRUE(ImportDLPUtilsPDensNode::read(referenceData, "dlpoly/benzene181/benzene181.11.pdens.zOrient0_10"));
    EXPECT_TRUE(checkData3D(osdf->sdf(), "SDF", referenceData, "dlpoly/benzene181/benzene181.11.pdens.zOrient0_10", 0.09));
}
} // namespace UnitTest
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/voxelDensity.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class VoxelDensityNodeTest : public ::testing::Test
{
    public:
    VoxelDensityNodeTest()
    {
    }

    protected:
    TestGraph testGraph_;
    VoxelDensityNode *voxelDensity_{nullptr};

    protected:
    // Set up the test graph
    void setUp(std::string species, int population, double density, std::string referenceCoordinatesFile)
    {
        ASSERT_TRUE(testGraph_.createConfiguration("Box", {{species, population}}, density));
        ASSERT_TRUE(testGraph_.appendSetCoordinates("ImportXYZStructure", referenceCoordinatesFile));

        // Add the analysis node to the graph
        voxelDensity_ = dynamic_cast<VoxelDensityNode *>(testGraph_.createNode("VoxelDensity"));
        ASSERT_TRUE(voxelDensity_);
        ASSERT_TRUE(testGraph_.addEdge({testGraph_.fetchHeadName(), "Configuration", "VoxelDensity", "Configuration"}));
    }
};

TEST_F(VoxelDensityNodeTest, Water)
{
    setUp("He", 512, 1.0, "dissolve/input/voxelDensity-helium.xyz");

    for (auto property : {VoxelDensityNode::TargetPropertyType::Mass, VoxelDensityNode::TargetPropertyType::AtomicNumber, VoxelDensityNode::TargetPropertyType::ScatteringLengthDensity})
    {
        for (auto voxelSize : {1, 2, 4, 8})
        {
            // Set module options
            ASSERT_TRUE(voxelDensity_->setOption<Number>("RequestedSideLength", voxelSize));
            ASSERT_TRUE(voxelDensity_->setOption("TargetProperty", property));

            // Clear and run from the voxel node explicitly
            voxelDensity_->clearData();
            ASSERT_EQ(voxelDensity_->run(), NodeConstants::ProcessResult::Success);
            ASSERT_EQ(voxelDensity_->versionIndex(), 0);

            // Check voxel count
            EXPECT_EQ(voxelDensity_->voxels().linearArray().size(), 512 / (voxelSize*voxelSize*voxelSize));

            // Check voxel values
            auto expectedValue = 0.0;
            for (auto value : histogram_.values())
        }
    }




    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        voxelDensity_->rdfBC(), "B-C RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", Data1DImportFileFormat::Data1DImportFormat::XY},
        4.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(voxelDensity_->angleABC(), "A-B-C angle",
                                                {"dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm",
                                                 Data1DImportFileFormat::Data1DImportFormat::XY},
                                                3.0e-6));
}

} // namespace UnitTest
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/voxelDensity.h"
#include "data/atomicMasses.h"
#include "io/export/data1D.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class VoxelDensityNodeTest : public ::testing::Test
{
    public:
    VoxelDensityNodeTest() {}

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

TEST_F(VoxelDensityNodeTest, Helium)
{
    setUp("He", 512, 1.0, "dissolve/input/voxelDensity-helium.xyz");

    const auto binWidth = 0.05;
    auto runCount = 0;

    for (auto &[property, expectedValue] : std::vector<std::pair<VoxelDensityNode::TargetPropertyType, double>>{
             {VoxelDensityNode::TargetPropertyType::Mass, AtomicMass::mass(Elements::He)},
             {VoxelDensityNode::TargetPropertyType::AtomicNumber, Elements::He},
             {VoxelDensityNode::TargetPropertyType::ScatteringLengthDensity,
              Sears91::boundCoherent(Sears91::naturalIsotope(Elements::He))}})
    {
        std::cout << std::format("Target property is {}\n", VoxelDensityNode::targetPropertyTypes().keyword(property));

        for (auto voxelSize : {1, 2, 4, 8})
        {
            // Set module options
            ASSERT_TRUE(voxelDensity_->setOption<Number>("VoxelSideLength", voxelSize));
            ASSERT_TRUE(voxelDensity_->setOption("TargetProperty", property));

            // Clear and run from the voxel node explicitly
            voxelDensity_->clearData();
            ASSERT_EQ(voxelDensity_->run(), NodeConstants::ProcessResult::Success);
            ASSERT_EQ(voxelDensity_->versionIndex(), runCount++);

            // Check voxel count
            auto voxelCount = 512 / (voxelSize * voxelSize * voxelSize);
            EXPECT_EQ(voxelDensity_->voxels().linearArray().size(), voxelCount);

            // Check for peak value in resulting data
            auto values = voxelDensity_->values();
            EXPECT_NEAR(values.value(expectedValue / binWidth), voxelCount, 1.0e-6);
        }
    }
}

} // namespace UnitTest
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/voxelDensity.h"
#include "data/atomicMasses.h"
#include "nodes/configuration.h"
#include "tests/testing.h"

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
    setUp("He", 512, 1.0, "xyz/voxelDensity-helium.xyz");

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
            // Set node options
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

TEST_F(VoxelDensityNodeTest, Water)
{
    setUp("species/water-dlpoly.toml", 267, 0.1, "dlpoly/water267-analysis/water-267-298K.xyz");

    const Vector3 range(-2.0, 10.0, 0.05);
    auto runCount = 0;

    for (auto &[property, expectedValue] : std::vector<std::pair<VoxelDensityNode::TargetPropertyType, double>>{
             {VoxelDensityNode::TargetPropertyType::Mass,
              (AtomicMass::mass(Elements::O) + 2.0 * AtomicMass::mass(Elements::H))},
             {VoxelDensityNode::TargetPropertyType::AtomicNumber, Elements::O + 2 * Elements::H},
             {VoxelDensityNode::TargetPropertyType::ScatteringLengthDensity,
              Sears91::boundCoherent(Sears91::naturalIsotope(Elements::O)) +
                  2.0 * Sears91::boundCoherent(Sears91::naturalIsotope(Elements::H))}})
    {
        std::cout << std::format("Target property is {}\n", VoxelDensityNode::targetPropertyTypes().keyword(property));

        // Set node options
        ASSERT_TRUE(voxelDensity_->setOption<Number>("VoxelSideLength", 20.0083));
        ASSERT_TRUE(voxelDensity_->setOption("TargetProperty", property));
        ASSERT_TRUE(voxelDensity_->setOption("Range", range));

        // Clear and run from the voxel node explicitly
        voxelDensity_->clearData();
        ASSERT_EQ(voxelDensity_->run(), NodeConstants::ProcessResult::Success);
        ASSERT_EQ(voxelDensity_->versionIndex(), runCount++);

        // Check voxel count == 1 (full box)
        EXPECT_EQ(voxelDensity_->voxels().linearArray().size(), 1);

        // Get box volume
        auto *configurationNode = dynamic_cast<ConfigurationNode *>(testGraph_.findNode("Box"));
        ASSERT_TRUE(configurationNode);
        const auto &box = configurationNode->configuration().box();

        // Check for peak value in resulting data
        auto values = voxelDensity_->values();
        EXPECT_NEAR(values.value(((267 * expectedValue / box.volume()) - range.x) / range.z), 1, 1.0e-6);
    }
}

} // namespace UnitTest

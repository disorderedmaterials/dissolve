// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "classes/atom.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"
#include "keywords/integer.h"
#include "keywords/vec3Double.h"
#include "module/types.h"
#include "tests/testData.h"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{

// Helium Array
class VoxelDensityModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    const double massHelium{4.002602};
    const int zHelium{2};
    const double scatteringLengthDensityHelium{3.26};
};

TEST_F(VoxelDensityModuleTest, ConfigurationTest)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-config.txt"));

    auto *cfg = systemTest.coreData().configurations().front().get();
    EXPECT_EQ(cfg->nAtoms(), 512);
    EXPECT_EQ(cfg->nMolecules(), 512);
    ASSERT_TRUE(cfg->atomicDensity());
    EXPECT_NEAR(*cfg->atomicDensity(), 1.0, 1.0e-6);
}

TEST_F(VoxelDensityModuleTest, Mass)
{
    // Test for number of voxels along each axis = 1, 2, 4, 8, 16
    for (auto n = 0; n < 4; ++n)
    {
        const auto nAxisVoxels = DissolveMath::power(2, n);
        auto nVoxels = DissolveMath::power(nAxisVoxels, 3);

        ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-config.txt"));

        // Grab module and perform setup
        auto *module = systemTest.getModule<VoxelDensityModule>("VoxelDensity");
        module->keywords().set("NAxisVoxels", nAxisVoxels);
        module->keywords().setEnumeration("TargetProperty", VoxelDensityModule::TargetPropertyType::Mass);

        // Iterate
        ASSERT_TRUE(systemTest.iterateRestart(1));

        // Check data
        const auto &data = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D")->get();
        auto maxBin = const_cast<Data1D *>(&data)->maxValueAt();
        auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        ASSERT_NEAR(maxBin.first * (*binRange).z, VoxelDensityModuleTest::massHelium, 10e-2);
        ASSERT_EQ(maxBin.second, nVoxels);
    }
}

TEST_F(VoxelDensityModuleTest, AtomicNumber)
{
    // Test for number of voxels along each axis = 8
    const auto nAxisVoxels = DissolveMath::power(2, 3);
    auto nVoxels = DissolveMath::power(nAxisVoxels, 3);

    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-atomicNumber-nVox8.txt"));

    // Grab module and perform setup
    auto *module = systemTest.getModule<VoxelDensityModule>("VoxelDensity");

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    // Check data
    const auto &data = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D")->get();
    auto maxBin = const_cast<Data1D *>(&data)->maxValueAt();
    auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_EQ(maxBin.first * (*binRange).z, VoxelDensityModuleTest::zHelium);
    ASSERT_EQ(maxBin.second, nVoxels);
}

TEST_F(VoxelDensityModuleTest, ScatteringLengthDensity)
{
    // Test for number of voxels along each axis = 8
    const auto nAxisVoxels = DissolveMath::power(2, 3);
    auto nVoxels = DissolveMath::power(nAxisVoxels, 3);

    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-sLD-nVox8.txt"));

    // Grab module and perform setup
    auto *module = systemTest.getModule<VoxelDensityModule>("VoxelDensity");

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    // Check data
    const auto &data = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D")->get();
    auto maxBin = const_cast<Data1D *>(&data)->maxValueAt();
    auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_NEAR(maxBin.first * (*binRange).z, VoxelDensityModuleTest::scatteringLengthDensityHelium, 10e-3);
    ASSERT_EQ(maxBin.second, nVoxels);
}

/*
TEST_F(VoxelDensityModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-water.txt"));
    auto module = systemTest.getModule<VoxelDensityModule>("AtomicMass");
    ASSERT_TRUE(module->keywords().getInt("NAxisVoxels") == 1);
}
*/
} // namespace UnitTest

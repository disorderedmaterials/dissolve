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
    // ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-mass-nVox8.txt"));
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-mass.txt"));

    // Grab module and perform setup
    VoxelDensityModule *modules[] = {systemTest.getModule<VoxelDensityModule>("VoxelDensity(1-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(2-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(4-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(8-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(16-bin)")};

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    // --> loop-based version: probably workable but ignore for now, in favour of a sequential series of assertions below
    /*
    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512, 4096)
    for (int n = 0; n <= 4; n++)
    {
        auto nAxisVoxels = DissolveMath::power(2, n);
        const auto &data = systemTest.dissolve().processingModuleData().search<const
    Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))->get(); auto maxBin = const_cast<Data1D
    *>(&data)->maxValueAt(); auto binRange = modules[n]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        ASSERT_NEAR(maxBin.first * (*binRange).z, VoxelDensityModuleTest::massHelium, 10e-2);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
    */

    // Check data
    const auto &data1 = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity(1-bin)//Data1D")->get();
    auto maxBin1 = const_cast<Data1D *>(&data1)->maxValueAt();
    auto binRange1 = modules[0]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_NEAR(maxBin1.first * (*binRange1).z, VoxelDensityModuleTest::massHelium, 10e-2);
    ASSERT_EQ(maxBin1.second, DissolveMath::power(1, 3));

    // Check data
    const auto &data2 = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity(2-bin)//Data1D")->get();
    auto maxBin2 = const_cast<Data1D *>(&data2)->maxValueAt();
    auto binRange2 = modules[1]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_NEAR(maxBin2.first * (*binRange2).z, VoxelDensityModuleTest::massHelium, 10e-2);
    ASSERT_EQ(maxBin2.second, DissolveMath::power(2, 3));

    // Check data
    const auto &data4 = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity(4-bin)//Data1D")->get();
    auto maxBin4 = const_cast<Data1D *>(&data4)->maxValueAt();
    auto binRange4 = modules[2]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_NEAR(maxBin4.first * (*binRange4).z, VoxelDensityModuleTest::massHelium, 10e-2);
    ASSERT_EQ(maxBin4.second, DissolveMath::power(4, 3));

    // Check data
    const auto &data8 = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity(8-bin)//Data1D")->get();
    auto maxBin8 = const_cast<Data1D *>(&data8)->maxValueAt();
    auto binRange8 = modules[3]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_NEAR(maxBin8.first * (*binRange8).z, VoxelDensityModuleTest::massHelium, 10e-2);
    ASSERT_EQ(maxBin8.second, DissolveMath::power(8, 3));

    // THIS FAILS
    // Check data
    const auto &data16 =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity(16-bin)//Data1D")->get();
    auto maxBin16 = const_cast<Data1D *>(&data16)->maxValueAt();
    auto binRange16 = modules[4]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
    ASSERT_NEAR(maxBin16.first * (*binRange16).z, VoxelDensityModuleTest::massHelium, 10e-2);
    ASSERT_EQ(maxBin16.second, DissolveMath::power(16, 3));
}
/*
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
*/
/*
TEST_F(VoxelDensityModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-water.txt"));
    auto module = systemTest.getModule<VoxelDensityModule>("AtomicMass");
    ASSERT_TRUE(module->keywords().getInt("NAxisVoxels") == 1);
}
*/
} // namespace UnitTest

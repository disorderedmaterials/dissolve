// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/limits.h"
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
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-mass.txt"));

    // Grab module and perform setup
    VoxelDensityModule *modules[] = {systemTest.getModule<VoxelDensityModule>("VoxelDensity(1-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(2-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(4-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(8-bin)")};

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512)
    for (int n = 0; n < 4; n++)
    {
        auto nAxisVoxels = DissolveMath::power(2, n);
        const auto &data = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))
                               ->get();
        auto maxBin = Limits::maxValueAt<double>(const_cast<Data1D *>(&data)->values());
        auto binRange = modules[n]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        ASSERT_NEAR(maxBin.first * (*binRange).z, VoxelDensityModuleTest::massHelium, 10e-2);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
}

TEST_F(VoxelDensityModuleTest, AtomicNumber)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-Z.txt"));

    // Grab module and perform setup
    VoxelDensityModule *modules[] = {systemTest.getModule<VoxelDensityModule>("VoxelDensity(1-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(2-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(4-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(8-bin)")};

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512)
    for (int n = 0; n < 4; n++)
    {
        auto nAxisVoxels = DissolveMath::power(2, n);
        const auto &data = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))
                               ->get();
        auto maxBin = Limits::maxValueAt<double>(const_cast<Data1D *>(&data)->values());
        auto binRange = modules[n]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        ASSERT_EQ(maxBin.first * (*binRange).z, VoxelDensityModuleTest::zHelium);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
}

TEST_F(VoxelDensityModuleTest, ScatteringLengthDensity)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-sLD.txt"));

    // Grab module and perform setup
    VoxelDensityModule *modules[] = {systemTest.getModule<VoxelDensityModule>("VoxelDensity(1-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(2-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(4-bin)"),
                                     systemTest.getModule<VoxelDensityModule>("VoxelDensity(8-bin)")};

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512)
    for (int n = 0; n < 4; n++)
    {
        auto nAxisVoxels = DissolveMath::power(2, n);
        const auto &data = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))
                               ->get();
        auto maxBin = Limits::maxValueAt<double>(const_cast<Data1D *>(&data)->values());
        auto binRange = modules[n]->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        ASSERT_NEAR(maxBin.first * (*binRange).z, VoxelDensityModuleTest::scatteringLengthDensityHelium, 10e-3);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
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

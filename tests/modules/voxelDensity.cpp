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
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

namespace UnitTest
{
#define BOX_LENGTH 8
#define NUM_ATOMS static_cast<int>(std::pow(BOX_LENGTH, 3))
#define MASS_HELIUM 4.002602
#define Z_HELIUM 2
#define SCATTERING_LENGTH_DENSITY_HELIUM 3.26

// Helium Array
class VoxelDensityModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(VoxelDensityModuleTest, ConfigurationTest)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium.txt"));

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
        Vec3<double> defaultBinRange(0.0, 10.0, 0.05);

        ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium.txt"));

        // Grab module and perform setup
        auto *module = systemTest.getModule<VoxelDensityModule>("VoxelDensity");
        module->keywords().set("NAxisVoxels", nAxisVoxels);
        module->keywords().setEnumeration("TargetProperty", VoxelDensityModule::TargetPropertyType::Mass);

        // Iterate
        ASSERT_TRUE(systemTest.iterateRestart(1));

        // Check data
        const auto &data = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D")->get();
        auto maxBin = const_cast<Data1D *>(&data)->maxValueAt();
        auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange").value_or(defaultBinRange);
        ASSERT_NEAR(maxBin.first * binRange.z, MASS_HELIUM, 10e2);
        ASSERT_EQ(maxBin.second, std::pow(nAxisVoxels, 3));
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

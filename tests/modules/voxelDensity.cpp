// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "classes/atom.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"
#include "keywords/double.h"
#include "keywords/vec3Double.h"
#include "math/limitsFunc.h"
#include "module/types.h"
#include "tests/testData.h"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{

class VoxelDensityModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    friend struct Helium;
    friend struct Water;

    // Helium test constants
    struct Helium
    {
        int boxSideLength;
        double mass;
        int Z;
        double scatteringLengthDensity;
        VoxelDensityModule *modules[4];
        Helium(VoxelDensityModuleTest &test) : boxSideLength(8), mass(4.002602), Z(2), scatteringLengthDensity(3.26), modules
                        {test.systemTest.getModule<VoxelDensityModule>("VoxelDensity(8-bin)"),
                         test.systemTest.getModule<VoxelDensityModule>("VoxelDensity(4-bin)"),
                         test.systemTest.getModule<VoxelDensityModule>("VoxelDensity(2-bin)"),
                         test.systemTest.getModule<VoxelDensityModule>("VoxelDensity(1-bin)")} {}
    };

    // Water test constants
    struct Water
    {
        int boxSideLength;
        double mass;
        int Z;
        VoxelDensityModule *modules[2];
        Water(VoxelDensityModuleTest &test) : boxSideLength(20.0083), mass(18.015), Z(10), modules
                        {test.systemTest.getModule<VoxelDensityModule>("VoxelDensity(Mass)"),
                         test.systemTest.getModule<VoxelDensityModule>("VoxelDensity(AtomicNumber)")} {}
    };

    // Return instance of test constants
    template<class T> const T constants() { return T(*this); }
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

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    auto testData = constants<VoxelDensityModuleTest::Helium>();
    auto start = DissolveMath::log2(testData.boxSideLength);

    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512)
    for (int n = start; n > 0; --n)
    {
        auto module = testData.modules[n];

        // Check that the voxel side length is as expected for each case
        auto expectedVoxelSideLength = module->keywords().get<double, DoubleKeyword>("VoxelSideLength");
        EXPECT_EQ((*expectedVoxelSideLength), double(DissolveMath::power(2, n)));

        auto nAxisVoxels = DissolveMath::power(2, 3-start);
        const auto &data = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))
                               ->get();
        auto maxBin = DissolveLimits::maxValueAt<double>(const_cast<Data1D *>(&data)->values());
        auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        auto binWidth = (*binRange).z;
        ASSERT_NEAR(maxBin.first * binWidth, testData.mass, 10e-2);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
}

TEST_F(VoxelDensityModuleTest, AtomicNumber)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-Z.txt"));

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    auto testData = constants<VoxelDensityModuleTest::Helium>();
    auto start = DissolveMath::log2(testData.boxSideLength);

    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512)
    for (int n = start; n > 0; --n)
    {
        auto module = testData.modules[n];

        // Check that the voxel side length is as expected for each case
        auto expectedVoxelSideLength = module->keywords().get<double, DoubleKeyword>("VoxelSideLength");
        EXPECT_EQ((*expectedVoxelSideLength), double(DissolveMath::power(2, n)));

        auto nAxisVoxels = DissolveMath::power(2, 3-start);
        const auto &data = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))
                               ->get();
        auto maxBin = DissolveLimits::maxValueAt<double>(const_cast<Data1D *>(&data)->values());
        auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        auto binWidth = (*binRange).z;
        ASSERT_EQ(maxBin.first * binWidth, testData.Z);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
}

TEST_F(VoxelDensityModuleTest, ScatteringLengthDensity)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium-sLD.txt"));

    // Iterate
    ASSERT_TRUE(systemTest.iterateRestart(1));

    auto testData = constants<VoxelDensityModuleTest::Helium>();
    auto start = DissolveMath::log2(testData.boxSideLength);

    // Check data for each module instance containing different numbers of voxels (1, 8, 64, 512)
    for (int n = start; n > 0; --n)
    {
        auto module = testData.modules[n];

        // Check that the voxel side length is as expected for each case
        auto expectedVoxelSideLength = module->keywords().get<double, DoubleKeyword>("VoxelSideLength");
        EXPECT_EQ((*expectedVoxelSideLength), double(DissolveMath::power(2, n)));

        auto nAxisVoxels = DissolveMath::power(2, 3-start);
        const auto &data = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>(fmt::format("VoxelDensity({}-bin)//Data1D", nAxisVoxels))
                               ->get();
        auto maxBin = DissolveLimits::maxValueAt<double>(const_cast<Data1D *>(&data)->values());
        auto binRange = module->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange");
        auto binWidth = (*binRange).z;
        ASSERT_NEAR(maxBin.first * binWidth, testData.scatteringLengthDensity, 10e-3);
        ASSERT_EQ(maxBin.second, DissolveMath::power(nAxisVoxels, 3));
    }
}

TEST_F(VoxelDensityModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-water.txt"));
    auto *cfg = systemTest.coreData().configurations().front().get();
    EXPECT_EQ(cfg->nMolecules(), 267);

    // Iterate for 95 frames
    ASSERT_TRUE(systemTest.iterateRestart(95));

    auto testData = constants<VoxelDensityModuleTest::Water>();
    auto moduleMass = testData.modules[0];
    auto moduleZ = testData.modules[1];

    const auto &dataMass = systemTest.dissolve()
                        .processingModuleData()
                        .search<const Data1D>(fmt::format("VoxelDensity(Mass)//Data1D"))
                        ->get();
    auto maxBinMass = DissolveLimits::maxValueAt<double>(const_cast<Data1D *>(&dataMass)->values());
    auto binWidthMass = (*moduleMass->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange")).z;
    EXPECT_EQ(maxBinMass.second, 1.0);
    ASSERT_NEAR(maxBinMass.first * binWidthMass, testData.mass*cfg->nMolecules()/(moduleMass->voxelVolume()), 10e-2);

    const auto &dataZ = systemTest.dissolve()
                        .processingModuleData()
                        .search<const Data1D>(fmt::format("VoxelDensity(AtomicNumber)//Data1D"))
                        ->get();
    auto maxBinZ = DissolveLimits::maxValueAt<double>(const_cast<Data1D *>(&dataZ)->values());
    auto binWidthZ = (*moduleZ->keywords().get<Vec3<double>, Vec3DoubleKeyword>("BinRange")).z;
    EXPECT_EQ(maxBinZ.second, 1.0);
    ASSERT_NEAR(maxBinZ.first * binWidthZ, testData.Z*cfg->nMolecules()/(moduleZ->voxelVolume()), 10e-2);
}
} // namespace UnitTest

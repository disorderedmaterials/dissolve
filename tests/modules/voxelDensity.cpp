// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "classes/atom.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"
#include "keywords/integer.h"
#include "module/types.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

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
    for (auto i = 0; i < 4; ++i)
    {
        const auto nPartitions = DissolveMath::power(2, i);

        ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-helium.txt"));

        // Grab module and perform setup
        auto *module = systemTest.getModule<VoxelDensityModule>("VoxelDensity");
        module->keywords().set("NumberOfPoints", nPartitions);
        module->keywords().setEnumeration("TargetProperty", VoxelDensityModule::TargetPropertyType::Mass);

        // Iterate
        ASSERT_TRUE(systemTest.iterateRestart(1));

        // Check data
        const auto &data8Bin =
            systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//Mass/A^3")->get();
        ASSERT_TRUE(data8Bin.nonZeroValues().size() == 1);
        auto maxBin = const_cast<Data1D *>(&data8Bin)->maxValueAt();
        ASSERT_NEAR(maxBin.first, MASS_HELIUM, 10e2);
        ASSERT_EQ(maxBin.second, std::pow(8, 3));
    }
}


//TEST_F(VoxelDensityModuleTest, HeliumBoxAtomicNumber)
//{
//    HeliumBox box(systemTest.coreData());
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 8);
//    systemTest.dissolve().iterate(1);
//    const auto &data8Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
//    ASSERT_TRUE(data8Bin.nonZeroValues().size() == 1);
//    auto max8Bin = const_cast<Data1D *>(&data8Bin)->maxValueAt();
//    ASSERT_EQ(max8Bin.first, Z_HELIUM);
//    ASSERT_EQ(max8Bin.second, std::pow(8, 3));
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 4);
//    systemTest.dissolve().iterate(20);
//    const auto &data4Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
//    ASSERT_TRUE(data4Bin.nonZeroValues().size() == 1);
//    auto max4Bin = const_cast<Data1D *>(&data4Bin)->maxValueAt();
//    ASSERT_EQ(max4Bin.first, Z_HELIUM);
//    ASSERT_EQ(max4Bin.second, std::pow(4, 3));
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 2);
//    systemTest.dissolve().iterate(20);
//    const auto &data2Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
//    ASSERT_TRUE(data2Bin.nonZeroValues().size() == 1);
//    auto max2Bin = const_cast<Data1D *>(&data2Bin)->maxValueAt();
//    ASSERT_EQ(max2Bin.first, Z_HELIUM);
//    ASSERT_EQ(max2Bin.second, std::pow(2, 3));
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 1);
//    systemTest.dissolve().iterate(20);
//    const auto &data1Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
//    ASSERT_TRUE(data1Bin.nonZeroValues().size() == 1);
//    auto max1Bin = const_cast<Data1D *>(&data1Bin)->maxValueAt();
//    ASSERT_EQ(max1Bin.first, Z_HELIUM);
//    ASSERT_EQ(max1Bin.second, 1);
//}
//
//TEST_F(VoxelDensityModuleTest, HeliumBoxScatteringLengthDensity)
//{
//    HeliumBox box(systemTest.coreData());
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 8);
//    systemTest.dissolve().iterate(1);
//    const auto &data8Bin = systemTest.dissolve()
//                               .processingModuleData()
//                               .search<const Data1D>("VoxelDensity//Data1D//ScatteringLengthDensity/A^3")
//                               ->get();
//    ASSERT_TRUE(data8Bin.nonZeroValues().size() == 1);
//    auto max8Bin = const_cast<Data1D *>(&data8Bin)->maxValueAt();
//    ASSERT_NEAR(max8Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM, 10e2);
//    ASSERT_EQ(max8Bin.second, std::pow(8, 3));
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 4);
//    systemTest.dissolve().iterate(20);
//    const auto &data4Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensity/A^3")->get();
//    ASSERT_TRUE(data4Bin.nonZeroValues().size() == 1);
//    auto max4Bin = const_cast<Data1D *>(&data4Bin)->maxValueAt();
//    ASSERT_NEAR(max4Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM, 10e2);
//    ASSERT_EQ(max4Bin.second, std::pow(4, 3));
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 2);
//    systemTest.dissolve().iterate(20);
//    const auto &data2Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensity/A^3")->get();
//    ASSERT_TRUE(data2Bin.nonZeroValues().size() == 1);
//    auto max2Bin = const_cast<Data1D *>(&data2Bin)->maxValueAt();
//    ASSERT_NEAR(max2Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM, 10e2);
//    ASSERT_EQ(max2Bin.second, std::pow(2, 3));
//
//    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 1);
//    systemTest.dissolve().iterate(20);
//    const auto &data1Bin =
//        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensity/A^3")->get();
//    ASSERT_TRUE(data1Bin.nonZeroValues().size() == 1);
//    auto max1Bin = const_cast<Data1D *>(&data1Bin)->maxValueAt();
//    ASSERT_NEAR(max1Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM, 10e2);
//    ASSERT_EQ(max1Bin.second, 1);
//}

TEST_F(VoxelDensityModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-water.txt"));
    auto module = systemTest.getModule<VoxelDensityModule>("AtomicMass");
    ASSERT_TRUE(module->keywords().getInt("NumberOfPoints") == 1);
}

} // namespace UnitTest

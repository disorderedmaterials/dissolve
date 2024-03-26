// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class NeutronSQModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(NeutronSQModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/correlations-water.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Total F(Q)
    EXPECT_TRUE(systemTest.checkData1D(
        "D2O//WeightedSQ//Total",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "H2O//WeightedSQ//Total",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "HDO//WeightedSQ//Total",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-5));
}

TEST_F(NeutronSQModuleTest, WaterMethanol)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/correlations-waterMethanol.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Total F(Q)
    // u01 file: 1  2   4   6   8  10  12  14  16
    //           Q HHH H5H DHH HDH HHD DDH HDD DDD
    EXPECT_TRUE(systemTest.checkData1D(
        "HHH//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "H5H//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "DHH//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "HDH//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "HHD//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 5.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "DDH//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 8.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "HDD//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 5.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "DDD//WeightedSQ//Total",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 5.0e-4));
}

TEST_F(NeutronSQModuleTest, WaterReferenceFT)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/epsr-water-3n-x.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(systemTest.checkData1D(
        "D2O//ReferenceDataFT",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-5));
    EXPECT_TRUE(systemTest.checkData1D(
        "H2O//ReferenceDataFT",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-5));
    EXPECT_TRUE(systemTest.checkData1D(
        "HDO//ReferenceDataFT",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 5.0e-5));
    EXPECT_TRUE(systemTest.checkData1D(
        "H2Ox//ReferenceDataFT",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 5.0e-5));
}

TEST_F(NeutronSQModuleTest, Benzene)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/epsr-benzene-3n.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Total F(Q)
    EXPECT_TRUE(systemTest.checkData1D(
        "C6H6//WeightedSQ//Total",
        {"epsr25/benzene200-neutron/benzene.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "C6D6//WeightedSQ//Total",
        {"epsr25/benzene200-neutron/benzene.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "5050//WeightedSQ//Total",
        {"epsr25/benzene200-neutron/benzene.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-3));
}
} // namespace UnitTest

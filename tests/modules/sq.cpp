// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class SQModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(SQModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/correlations-water.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Partial S(Q) (unbound terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-OW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 3.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//HW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 3.0e-3));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//HW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-5));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-OW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
}

TEST_F(SQModuleTest, WaterMethanol)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/correlations-waterMethanol.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    /*
     * Partial S(Q)
     *  Order of partials in EPSR files is:
     *    2      4      6      8      10     12     14     16     18     20     22     24     26     28     30     32     34
     *  OW-OW  OW-HW  OW-CT  OW-HC  OW-OH  OW-HO  HW-HW  HW-CT  HW-HC  HW-OH  HW-HO  CT-CT  CT-HC  CT-OH  CT-HO  HC-HC  HC-OH
     *    36     38     40     42
     *  HC-HO  OH-OH  OH-HO  HO-HO
     */

    // Partial g(r) (unbound terms
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-OW//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 4.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-HW//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-2));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OW-CT//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OW-HC//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OW-OH//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OW-HO//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HW-HW//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 5.0e-2));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HW-CT//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HW-HC//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 18}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HW-OH//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 20}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HW-HO//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 22}));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//CT-CT//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 24}, 8.0e-3));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//CT-HC//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 26}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//CT-OH//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 28}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//CT-HO//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 30}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HC-HC//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 32}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HC-OH//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 34}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HC-HO//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 36}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OH-OH//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 38}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OH-HO//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 40}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HO-HO//Unbound", {"epsr25/water300methanol600/watermeth.EPSR.f01",
                                                                     Data1DImportFileFormat::Data1DImportFormat::XY, 1, 42}));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OW-HW//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HW-HW//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//CT-HC//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 26}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//CT-OH//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 28}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//CT-HO//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 30}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HC-HC//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 32}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HC-OH//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 34}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//HC-HO//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 36}));
    EXPECT_TRUE(
        systemTest.checkData1D("SQs//UnweightedSQ//OH-HO//Bound", {"epsr25/water300methanol600/watermeth.EPSR.s01",
                                                                   Data1DImportFileFormat::Data1DImportFormat::XY, 1, 40}));

    // Partial g(r) (intramolecular terms, zero)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-OW//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-CT//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-HC//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OW-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HW-CT//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HW-HC//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 18}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HW-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 20}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HW-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 22}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//CT-CT//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 24}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//OH-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 38}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HO-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 42}, 1.0e-5,
        Error::ErrorType::RMSEError));
}

TEST_F(SQModuleTest, Benzene)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-benzene-3n.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Partial S(Q) (unbound terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//CA-CA//Unbound",
        {"epsr25/benzene200-neutron/benzene.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 7.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//CA-HA//Unbound",
        {"epsr25/benzene200-neutron/benzene.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HA-HA//Unbound",
        {"epsr25/benzene200-neutron/benzene.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 8.0e-3));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//CA-CA//Bound",
        {"epsr25/benzene200-neutron/benzene.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.1e-2,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//CA-HA//Bound",
        {"epsr25/benzene200-neutron/benzene.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQs//UnweightedSQ//HA-HA//Bound",
        {"epsr25/benzene200-neutron/benzene.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-2));
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/gr/gr.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class GRModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(GRModuleTest, Methods)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/rdfMethod.txt"));
    auto *grModule = systemTest.getModule<GRModule>("GR01");

    // Simple method
    grModule->keywords().setEnumeration("Method", GRModule::PartialsMethod::SimpleMethod);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Cells method
    systemTest.coreData().configurations().front()->incrementContentsVersion();
    grModule->keywords().setEnumeration("Method", GRModule::PartialsMethod::CellsMethod);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
}

TEST_F(GRModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/correlations-water.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-OW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 6.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//HW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.1));
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//HW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.5e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-OW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
}

TEST_F(GRModuleTest, WaterMethanol)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/correlations-waterMethanol.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    /*
     * Partial Radial Distribution Functions
     *  Order of partials in EPSR files is:
     *    2      4      6      8      10     12     14     16     18     20     22     24     26     28     30     32     34
     *  OW-OW  OW-HW  OW-CT  OW-HC  OW-OH  OW-HO  HW-HW  HW-CT  HW-HC  HW-OH  HW-HO  CT-CT  CT-HC  CT-OH  CT-HO  HC-HC  HC-OH
     *    36     38     40     42
     *  HC-HO  OH-OH  OH-HO  HO-HO
     */

    // Partial g(r) (unbound terms
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-OW//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-HW//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.5));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-CT//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 0.2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-HC//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 7.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-OH//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 0.2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-HO//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 0.3));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-HW//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 0.4));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-CT//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 0.1));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-HC//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 18}, 4.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-OH//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 20}, 0.2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-HO//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 22}, 0.2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-CT//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 24}, 0.2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-HC//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 26}, 4.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-OH//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 28}, 0.1));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-HO//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 30}, 0.1));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HC-HC//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 32}, 4.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HC-OH//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 34}, 4.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HC-HO//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 36}, 5.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OH-OH//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 38}, 0.3));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OH-HO//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 40}, 0.1));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HO-HO//Unbound",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 42}, 0.3));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-HW//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.8));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-HW//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 0.5));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-HC//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 26}, 0.3));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 28}, 0.5));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 30}, 0.2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HC-HC//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 32}, 0.06));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HC-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 34}, 0.08));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HC-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 36}, 0.5));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OH-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 40}, 0.5));

    // Partial g(r) (intramolecular terms, zero)
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-OW//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-CT//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-HC//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OW-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-CT//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-HC//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 18}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 20}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HW-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 22}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//CT-CT//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 24}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//OH-OH//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 38}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Mix//OriginalGR//HO-HO//Bound",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 42}, 1.0e-5,
        Error::ErrorType::RMSEError));
}

TEST_F(GRModuleTest, Benzene)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/epsr-benzene-3n.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Liquid//OriginalGR//CA-CA//Unbound",
        {"epsr25/benzene200-neutron/benzene.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Liquid//OriginalGR//CA-HA//Unbound",
        {"epsr25/benzene200-neutron/benzene.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Liquid//OriginalGR//HA-HA//Unbound",
        {"epsr25/benzene200-neutron/benzene.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Liquid//OriginalGR//CA-CA//Bound",
        {"epsr25/benzene200-neutron/benzene.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 0.12));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Liquid//OriginalGR//CA-HA//Bound",
        {"epsr25/benzene200-neutron/benzene.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.18));
    EXPECT_TRUE(systemTest.checkData1D(
        "GRs//Liquid//OriginalGR//HA-HA//Bound",
        {"epsr25/benzene200-neutron/benzene.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 9.0e-2));
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class SiteRDFModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(SiteRDFModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/siteRDF-water.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    // O-O RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));

    // H1-H2 RDF, excluding intramolecular interactions
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H1-H2)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        5.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H1-H2)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        5.0e-3));

    // COM-COM RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(COM-COM)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(COM-COM)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));

    // Coordination numbers
    EXPECT_TRUE(systemTest.checkSampledDouble("coordination number A", "RDF(COM-COM)//Sum1D//CN//A", 4.32359551, 5.0e-3));
    EXPECT_TRUE(systemTest.checkSampledDouble("coordination number B", "RDF(COM-COM)//Sum1D//CN//B", 19.413049, 3.0e-2));
}

TEST_F(SiteRDFModuleTest, WaterNPT)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/siteRDF-waterNPT.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    // O-O RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)//Process1D//RDF",
        {"dlpoly/water267-npt/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)-Analyser//Process1D//RDF",
        {"dlpoly/water267-npt/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));

    // H1-H2 RDF, excluding intramolecular interactions
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H1-H2)//Process1D//RDF",
        {"dlpoly/water267-npt/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H1-H2)-Analyser//Process1D//RDF",
        {"dlpoly/water267-npt/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));

    // COM-COM RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(COM-COM)//Process1D//RDF",
        {"dlpoly/water267-npt/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 7.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(COM-COM)-Analyser//Process1D//RDF",
        {"dlpoly/water267-npt/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 7.0e-3));
}

TEST_F(SiteRDFModuleTest, WaterDynamic)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/siteRDF-waterDynamic.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    // O-O RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        9.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        9.0e-2));

    // H1-H2 RDF, excluding intramolecular interactions
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H-H)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_HHsum", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        3.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H-H)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_HHsum", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        3.0e-3));
}

TEST_F(SiteRDFModuleTest, WaterFragments)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/siteRDF-waterFragments.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    // O-O RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(OW-OW)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));

    // H1-H2 RDF, excluding intramolecular interactions
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H1-H2)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_HHsum", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        5.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(H1-H2)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_HHsum", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        5.0e-3));

    // COM-COM RDF
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(COM-COM)//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "RDF(COM-COM)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));

    // Coordination numbers
    EXPECT_TRUE(systemTest.checkSampledDouble("coordination number A", "RDF(COM-COM)//Sum1D//CN//A", 4.32359551, 5.0e-3));
    EXPECT_TRUE(systemTest.checkSampledDouble("coordination number B", "RDF(COM-COM)//Sum1D//CN//B", 19.413049, 3.0e-2));
}

} // namespace UnitTest

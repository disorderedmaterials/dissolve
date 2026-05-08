// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/epsr.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
Array2D<double> basicMatrix()
{
    // Set up and return the basic scattering matrix for all datasets plus space for simulated partials
    // The matrix elements for the partials are taken directly from EPSR and do not have any other weightings applied
    Array2D<double> W(8, 3, false);
    W.setRow(0, {3.7416454404592514E-002, -9.6432961523532867E-002, 6.2133874744176865E-002});
    W.setRow(1, {3.7416454404592514E-002, 3.7809770554304123E-002, 9.5518063753843307E-003});
    W.setRow(2, {3.7416454404592514E-002, 2.2180502128321677E-004, 3.2871542998691439E-007});
    W.setRow(3, {3.7416454404592514E-002, 0.17205251753330231, 0.19778776168823242});
    W.setRow(4, {0.32323204871932149, 0.16161840828362922, 2.0202599030345235E-002});
    W.setRow(5, {1.0, 0.0, 0.0});
    W.setRow(6, {0.0, 1.0, 0.0});
    W.setRow(7, {0.0, 0.0, 1.0});
    return W;
}
void testMatrices(const Array2D<double> &A, const Array2D<double> &B, double tolerance)
{
    ASSERT_EQ(A.nRows(), B.nRows());
    ASSERT_EQ(A.nColumns(), B.nColumns());
    for (auto r = 0; r < A.nRows(); ++r)
        for (auto c = 0; c < A.nColumns(); ++c)
        {
            auto a = A[{r, c}];
            auto b = B[{r, c}];
            EXPECT_NEAR(a, b, tolerance);
        }
}
bool testAbsoluteEPEnergy(const std::vector<double> &coefficients, double expectedMagnitude, double threshold)
{
    auto error = fabs(expectedMagnitude - (*std::max_element(coefficients.begin(), coefficients.end()) -
                                           *std::min_element(coefficients.begin(), coefficients.end())));
    auto isOK = error <= threshold;
    Messenger::print("Absolute EP magnitude has error of {} with reference value "
                     "and is {} (threshold is {})\n",
                     error, isOK ? "OK" : "NOT OK", threshold);
    return isOK;
}

TEST(EPSRNodeTest, Water3NInpA)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Bulk", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/water1000-neutron/waterbox.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));
    auto importCoords = testGraph.head<ImportConfigurationCoordinatesNode>();

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::ShiftedShortRangeTruncation);
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::ShiftedCoulombTruncation);
    PairPotential::setRange(12.0, 0.005);

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.025));
    ASSERT_TRUE(grNode->setOption("Averaging", std::optional<Number>(0)));
    ASSERT_TRUE(grNode->setOption<Function1DWrapper>("IntraBroadening", {Functions1D::Form::None}));
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::AutoMethod));

    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<Averaging::AveragingScheme>("AveragingScheme", Averaging::LinearAveraging));
    ASSERT_TRUE(sqNode->setOption<Function1DWrapper>("QBroadening", {Functions1D::Form::OmegaDependentGaussian, {0.02}}));
    ASSERT_TRUE(sqNode->setOption("WindowFunction", WindowFunction::Form::None));

    // Add in NeutronSQ
    auto H2O = testGraph.appendNeutronSQ(
        sqNode, "H2O", {{"Water", "Natural", 1.0}}, {},
        {"epsr25/water1000-neutron/H2O.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(H2O);
    auto D2O = testGraph.appendNeutronSQ(
        sqNode, "D2O", {{"Water", "D2O", 1.0}}, {},
        {"epsr25/water1000-neutron/D2O.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(D2O);
    auto HDO = testGraph.appendNeutronSQ(
        sqNode, "HDO", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}}, Exchangeables({"HW"}),
        {"epsr25/water1000-neutron/HDO.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(HDO);

    for (const auto &neutronSQ : {H2O, D2O, HDO})
    {
        ASSERT_TRUE(neutronSQ->setOption("ReferenceFTQMax", std::optional<Number>(30.0)));
        ASSERT_TRUE(neutronSQ->setOption("ReferenceFTQMin", std::optional<Number>(0.3)));
        ASSERT_TRUE(neutronSQ->setOption("ReferenceNormalisedTo", StructureFactors::NormalisationType::NoNormalisation));
        ASSERT_TRUE(neutronSQ->setOption("ReferenceWindowFunction", WindowFunction::Form::Lorch0));
    }

    // Add EPSR
    auto epsrNode = static_cast<EPSRNode *>(testGraph.appendNode("EPSR", "EPSR01"));
    ASSERT_TRUE(epsrNode);
    ASSERT_TRUE(testGraph.addEdge({std::string(importCoords->name()), "Configuration", "EPSR01", "Configuration"}));
    ASSERT_TRUE(epsrNode->setOption("EReq", Number(1.0)));
    ASSERT_TRUE(epsrNode->setOption("ExpansionFunction", EPSRNode::PoissonExpansionFunction));
    ASSERT_TRUE(epsrNode->setOption("Feedback", Number(0.9)));
    ASSERT_TRUE(epsrNode->setOption("OverwritePotentials", true));
    ASSERT_TRUE(epsrNode->setOption<std::string>("InpAFile", "epsr25/water1000-neutron/water.EPSR.inpa"));
    ASSERT_TRUE(epsrNode->setOption("QMin", Number(1.5)));
    ASSERT_TRUE(epsrNode->setOption("Smoothing", std::optional<Number>(0)));
    ASSERT_TRUE(epsrNode->setOption("NPItSs", std::optional<Number>(0)));

    // Run graph
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(epsrNode->run(), NodeConstants::ProcessResult::Success);

    // Estimated Partials
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("OW", "OW"), "EPSR01_EstimatedSQ_OW-OW",
        {"epsr25/water1000-neutron/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("OW", "HW"), "EPSR01_EstimatedSQ_OW-HW",
        {"epsr25/water1000-neutron/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("HW", "HW"), "EPSR01_EstimatedSQ_HW-HW",
        {"epsr25/water1000-neutron/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-4));

    // DeltaFQ Fits
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(D2O).deltaFQFit, "EPSR01_DeltaFQFit_D2O",
        {"epsr25/water1000-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 7.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(H2O).deltaFQFit, "EPSR01_DeltaFQFit_H2O",
        {"epsr25/water1000-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(HDO).deltaFQFit, "EPSR01_DeltaFQFit_HDO",
        {"epsr25/water1000-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-4));

    // Generated Potentials
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->additionalPotential("OW", "OW"), "Dissolve_Potential_OW-OW_Additional",
        {"epsr25/water1000-neutron/water.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->additionalPotential("OW", "HW"), "Dissolve_Potential_OW-HW_Additional",
        {"epsr25/water1000-neutron/water.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->additionalPotential("HW", "HW"), "Dissolve_Potential_HW-HW_Additional",
        {"epsr25/water1000-neutron/water.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.1e-2));

    // Absolute magnitudes of EP
    auto cfg = epsrNode->getInputValue<Configuration *>("Configuration");
    auto &coefficients = epsrNode->potentialCoefficients(cfg->atomTypePopulations().size());
    ASSERT_FALSE(coefficients.empty());
    std::vector<std::tuple<int, int, double>> expectedEPMagnitudes = {{0, 0, 0.2475}, {0, 1, 0.3722}, {1, 1, 0.4567}};
    for (auto &&[i, j, epMag] : expectedEPMagnitudes)
    {
        testAbsoluteEPEnergy(coefficients[{i, j}], epMag, 1.0e-4);
    }
}

TEST(EPSRNodeTest, Water3NX)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Bulk", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/water1000-neutron-xray/waterbox.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));
    auto importCoords = testGraph.head<ImportConfigurationCoordinatesNode>();

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::ShiftedShortRangeTruncation);
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::ShiftedCoulombTruncation);
    PairPotential::setRange(15.0, 0.005);

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.025));
    ASSERT_TRUE(grNode->setOption("Averaging", std::optional<Number>(5)));
    ASSERT_TRUE(grNode->setOption<Function1DWrapper>("IntraBroadening", {Functions1D::Form::None}));

    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption("QMin", Number(0.01)));
    ASSERT_TRUE(sqNode->setOption<Function1DWrapper>("QBroadening", {Functions1D::Form::OmegaDependentGaussian, {0.02}}));

    // Add in NeutronSQ and XRaySQ
    auto H2Ox = testGraph.appendXRaySQ(
        sqNode, "H2Ox", {"epsr25/water1000-neutron-xray/PCCPfofq.txt", Data1DImportFileFormat::Data1DImportFormat::XY});
    ASSERT_TRUE(H2Ox);
    auto H2O = testGraph.appendNeutronSQ(
        sqNode, "H2O", {{"Water", "Natural", 1.0}}, {},
        {"epsr25/water1000-neutron-xray/H2O.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(H2O);
    auto D2O = testGraph.appendNeutronSQ(
        sqNode, "D2O", {{"Water", "D2O", 1.0}}, {},
        {"epsr25/water1000-neutron-xray/D2O.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(D2O);
    auto HDO = testGraph.appendNeutronSQ(
        sqNode, "HDO", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}}, Exchangeables({"HW"}),
        {"epsr25/water1000-neutron-xray/HDO.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(HDO);

    for (const auto &neutronSQ : {H2O, D2O, HDO})
    {
        ASSERT_TRUE(neutronSQ->setOption("ReferenceFTQMax", std::optional<Number>(30.0)));
        ASSERT_TRUE(neutronSQ->setOption("ReferenceFTQMin", std::optional<Number>(0.5)));
        ASSERT_TRUE(neutronSQ->setOption("ReferenceNormalisedTo", StructureFactors::NormalisationType::NoNormalisation));
        ASSERT_TRUE(neutronSQ->setOption("ReferenceWindowFunction", WindowFunction::Form::Lorch0));
    }

    ASSERT_TRUE(H2Ox->setOption("ReferenceFTQMin", std::optional<Number>(0.5)));
    ASSERT_TRUE(H2Ox->setOption("NormaliseTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption("ReferenceNormalisedTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));

    // Add EPSR
    auto epsrNode = static_cast<EPSRNode *>(testGraph.appendNode("EPSR", "EPSR01"));
    ASSERT_TRUE(epsrNode);
    ASSERT_TRUE(testGraph.addEdge({std::string(importCoords->name()), "Configuration", "EPSR01", "Configuration"}));
    ASSERT_TRUE(epsrNode->setOption("EReq", Number(6.0)));
    ASSERT_TRUE(epsrNode->setOption("Feedback", Number(0.9)));
    ASSERT_TRUE(epsrNode->setOption("QMin", Number(0.5)));
    ASSERT_TRUE(epsrNode->setOption("QMax", Number(30.0)));
    ASSERT_TRUE(epsrNode->setOption("NPItSs", std::optional<Number>(0)));

    // Run graph
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(H2Ox->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(epsrNode->run(), NodeConstants::ProcessResult::Success);

    // Test total neutron-weighted F(r)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(D2O).simulatedFR, "EPSR01_SimulatedFR_D2O",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(H2O).simulatedFR, "EPSR01_SimulatedFR_H2O",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 8.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(HDO).simulatedFR, "EPSR01_SimulatedFR_HDO",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(H2Ox).simulatedFR, "EPSR01_SimulatedFR_H2Ox",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 2.0e-2));

    // Estimated Partials
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("OW", "OW"), "EPSR01_EstimatedSQ_OW-OW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 6.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("OW", "HW"), "EPSR01_EstimatedSQ_OW-HW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("HW", "HW"), "EPSR01_EstimatedSQ_HW-HW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.3e-2));
}

TEST(EPSRNodeTest, Benzene)
{
    //  Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Liquid", {{"species/benzene.toml", 200}}, 0.876,
                                              Units::DensityUnits::GramsPerCentimetreCubedUnits));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/benzene200-neutron/boxbenz.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));
    auto importCoords = testGraph.head<ImportConfigurationCoordinatesNode>();

    // Adjust pair potential properties
    PairPotential::setRange(12.0);

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(grNode->setOption("Averaging", std::optional<Number>(5)));
    ASSERT_TRUE(grNode->setOption<Function1DWrapper>("IntraBroadening", {Functions1D::Form::None}));

    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<Function1DWrapper>("QBroadening", {Functions1D::Form::OmegaDependentGaussian, {0.02}}));
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Add in NeutronSQ
    auto C6H6 = testGraph.appendNeutronSQ(
        sqNode, "C6H6", {{"Benzene", "Natural", 1.0}}, {},
        {"epsr25/benzene200-neutron/C6H6.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(C6H6);
    auto C6D6 = testGraph.appendNeutronSQ(
        sqNode, "C6D6", {{"Benzene", "C6D6", 1.0}}, {},
        {"epsr25/benzene200-neutron/C6D6.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(C6D6);
    auto FiftyFifty = testGraph.appendNeutronSQ(
        sqNode, "5050", {{"Benzene", "Natural", 0.5}, {"Benzene", "C6D6", 0.5}}, {},
        {"epsr25/benzene200-neutron/5050.mint01", Data1DImportFileFormat::Data1DImportFormat::GudrunMint});
    ASSERT_TRUE(FiftyFifty);

    // Add EPSR
    auto epsrNode = static_cast<EPSRNode *>(testGraph.appendNode("EPSR", "EPSR01"));
    ASSERT_TRUE(epsrNode);
    ASSERT_TRUE(testGraph.addEdge({std::string(importCoords->name()), "Configuration", "EPSR01", "Configuration"}));
    ASSERT_TRUE(epsrNode->setOption("EReq", Number(3.0)));
    ASSERT_TRUE(epsrNode->setOption("Feedback", Number(0.9)));
    ASSERT_TRUE(epsrNode->setOption<std::string>("InpAFile", "epsr25/benzene200-neutron/benzene.EPSR.inpa"));
    ASSERT_TRUE(epsrNode->setOption("QMin", Number(0.05)));
    ASSERT_TRUE(epsrNode->setOption("Smoothing", std::optional<Number>(0)));
    ASSERT_TRUE(epsrNode->setOption("NPItSs", std::optional<Number>(0)));

    // Run graph
    ASSERT_EQ(C6H6->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(C6D6->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(FiftyFifty->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(epsrNode->run(), NodeConstants::ProcessResult::Success);

    // Test total neutron-weighted F(r)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(C6H6).simulatedFR, "EPSR01//SimulatedFR//C6H6",
        {"epsr25/benzene200-neutron/benzene.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.5e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(C6D6).simulatedFR, "EPSR01//SimulatedFR//C6D6",
        {"epsr25/benzene200-neutron/benzene.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.7e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(FiftyFifty).simulatedFR, "EPSR01//SimulatedFR//5050",
        {"epsr25/benzene200-neutron/benzene.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.1e-2));

    // Test partial S(Q) derived from experiment via matrix inversion
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("CA", "CA"), "EPSR01//EstimatedSQ//CA-CA",
        {"epsr25/benzene200-neutron/benzene.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("CA", "HA"), "EPSR01//EstimatedSQ//CA-HA",
        {"epsr25/benzene200-neutron/benzene.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->estimatedSQ("HA", "HA"), "EPSR01//EstimatedSQ//HA-HA",
        {"epsr25/benzene200-neutron/benzene.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));

    // DeltaFQ Fits
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(C6H6).deltaFQFit, "EPSR01//DeltaFQFit//C6H6",
        {"epsr25/benzene200-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(C6D6).deltaFQFit, "EPSR01//DeltaFQFit//C6D6",
        {"epsr25/benzene200-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->targetProcessData(FiftyFifty).deltaFQFit, "EPSR01//DeltaFQFit//5050",
        {"epsr25/benzene200-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 5.0e-4));

    // Generated potentials
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->additionalPotential("CA", "CA"), "Dissolve//Potential_CA-CA_Additional",
        {"epsr25/benzene200-neutron/benzene.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 8.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->additionalPotential("CA", "HA"), "Dissolve//Potential_CA-HA_Additional",
        {"epsr25/benzene200-neutron/benzene.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        epsrNode->additionalPotential("HA", "HA"), "Dissolve//Potential_HA-HA_Additional",
        {"epsr25/benzene200-neutron/benzene.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-3));

    // Absolute magnitudes of EP
    auto cfg = epsrNode->getInputValue<Configuration *>("Configuration");
    auto &coefficients = epsrNode->potentialCoefficients(cfg->atomTypePopulations().size());
    ASSERT_FALSE(coefficients.empty());
    std::vector<std::tuple<int, int, double>> expectedEPMagnitudes = {{0, 0, 0.4023}, {0, 1, 0.1966}, {1, 1, 0.2491}};
    for (auto &&[i, j, epMag] : expectedEPMagnitudes)
    {
        testAbsoluteEPEnergy(coefficients[{i, j}], epMag, 1.0e-4);
    }
}

} // namespace UnitTest
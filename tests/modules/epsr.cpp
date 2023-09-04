// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/epsr/epsr.h"
#include "classes/configuration.h"
#include "keywords/double.h"
#include "main/dissolve.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class EPSRModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    protected:
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
};

TEST_F(EPSRModuleTest, Water3NInpA)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-water-inpa.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Estimated Partials
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//OW-OW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 0.24));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//OW-HW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 8.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//HW-HW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.4e-2));

    // DeltaFQ Fits
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//DeltaFQFit//D2O", {"epsr25/water1000-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        7.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//DeltaFQFit//H2O", {"epsr25/water1000-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4},
        6.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//DeltaFQFit//HDO", {"epsr25/water1000-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6},
        4.0e-4));

    // Generated Potentials
    EXPECT_TRUE(systemTest.checkData1D(
        "Dissolve//Potential_OW-OW_Additional",
        {"epsr25/water1000-neutron/water.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "Dissolve//Potential_OW-HW_Additional",
        {"epsr25/water1000-neutron/water.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "Dissolve//Potential_HW-HW_Additional",
        {"epsr25/water1000-neutron/water.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.1e-2));

    // Absolute magnitudes of EP
    auto *epsrModule = systemTest.getModule<EPSRModule>("EPSR01");
    auto &coefficients = epsrModule->potentialCoefficients(systemTest.dissolve(), systemTest.coreData().nAtomTypes());
    ASSERT_FALSE(coefficients.empty());
    std::vector<std::tuple<int, int, double>> expectedEPMagnitudes = {{0, 0, 0.2475}, {0, 1, 0.3722}, {1, 1, 0.4567}};
    for (auto &&[i, j, epMag] : expectedEPMagnitudes)
    {
        testAbsoluteEPEnergy(coefficients[{i, j}], epMag, 1.0e-4);
    }
}

TEST_F(EPSRModuleTest, Water3NX)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-water-3n-x.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Test total neutron-weighted F(r)
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//D2O",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//H2O",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 8.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//HDO",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//H2Ox",
        {"epsr25/water1000-neutron-xray/water.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 2.0e-2));

    // Estimated Partials
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//OW-OW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 6.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//OW-HW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//HW-HW",
        {"epsr25/water1000-neutron-xray/water.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.3e-2));
}

TEST_F(EPSRModuleTest, Benzene)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-benzene-3n.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Test total neutron-weighted F(r)
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//C6H6",
        {"epsr25/benzene200-neutron/benzene.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.5e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//C6D6",
        {"epsr25/benzene200-neutron/benzene.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.7e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//SimulatedFR//5050",
        {"epsr25/benzene200-neutron/benzene.EPSR.x01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.1e-2));

    // Test partial S(Q) derived from experiment via matrix inversion
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//CA-CA",
        {"epsr25/benzene200-neutron/benzene.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//CA-HA",
        {"epsr25/benzene200-neutron/benzene.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//EstimatedSQ//HA-HA",
        {"epsr25/benzene200-neutron/benzene.EPSR.q01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));

    // DeltaFQ Fits
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//DeltaFQFit//C6H6",
        {"epsr25/benzene200-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//DeltaFQFit//C6D6",
        {"epsr25/benzene200-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "EPSR01//DeltaFQFit//5050",
        {"epsr25/benzene200-neutron/FQ.delfit", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 5.0e-4));

    // Generated potentials
    EXPECT_TRUE(systemTest.checkData1D(
        "Dissolve//Potential_CA-CA_Additional",
        {"epsr25/benzene200-neutron/benzene.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 8.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "Dissolve//Potential_CA-HA_Additional",
        {"epsr25/benzene200-neutron/benzene.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "Dissolve//Potential_HA-HA_Additional",
        {"epsr25/benzene200-neutron/benzene.EPSR.p01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-3));

    // Absolute magnitudes of EP
    auto *epsrModule = systemTest.getModule<EPSRModule>("EPSR01");
    auto &coefficients = epsrModule->potentialCoefficients(systemTest.dissolve(), systemTest.coreData().nAtomTypes());
    ASSERT_FALSE(coefficients.empty());
    std::vector<std::tuple<int, int, double>> expectedEPMagnitudes = {{0, 0, 0.4023}, {0, 1, 0.1966}, {1, 1, 0.2491}};
    for (auto &&[i, j, epMag] : expectedEPMagnitudes)
    {
        testAbsoluteEPEnergy(coefficients[{i, j}], epMag, 1.0e-4);
    }
}

TEST_F(EPSRModuleTest, BenzeneReadPCof) { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-benzene-3n-pcof.txt")); }

TEST_F(EPSRModuleTest, ScatteringMatrix)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-5datasets.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Find EPSR module
    auto *epsrModule = systemTest.getModule<EPSRModule>("EPSR01");

    // Grab scattering matrix
    auto &W = epsrModule->scatteringMatrix();

    // Set up standard scattering weights
    auto equalWeights = basicMatrix();
    auto feedback = epsrModule->keywords().get<double, DoubleKeyword>("Feedback").value_or(1.0);
    equalWeights *= feedback;
    equalWeights.setRow(5, {1.0 - feedback, 0.0, 0.0});
    equalWeights.setRow(6, {0.0, 1.0 - feedback, 0.0});
    equalWeights.setRow(7, {0.0, 0.0, 1.0 - feedback});
    testMatrices(equalWeights, W.matrix(), 1.0e-6);
}

TEST_F(EPSRModuleTest, DataWeighting)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-5datasets-weighted.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Find EPSR module
    auto *epsrModule = systemTest.getModule<EPSRModule>("EPSR01");

    // Grab scattering matrix
    auto &W = epsrModule->scatteringMatrix();

    // Set up scattering weights matrix with a five-fold increase in weight for the x-ray dataset
    auto unequalWeights = basicMatrix();
    auto feedback = epsrModule->keywords().get<double, DoubleKeyword>("Feedback").value_or(1.0);
    unequalWeights *= feedback;
    unequalWeights.multiplyRow(0, 5.0 / 9.0);
    unequalWeights.multiplyRow(1, 5.0 / 9.0);
    unequalWeights.multiplyRow(2, 5.0 / 9.0);
    unequalWeights.multiplyRow(3, 5.0 / 9.0);
    unequalWeights.multiplyRow(4, 25.0 / 9.0);
    unequalWeights.setRow(5, {1.0 - feedback, 0.0, 0.0});
    unequalWeights.setRow(6, {0.0, 1.0 - feedback, 0.0});
    unequalWeights.setRow(7, {0.0, 0.0, 1.0 - feedback});
    testMatrices(unequalWeights, W.matrix(), 1.0e-6);
}

} // namespace UnitTest

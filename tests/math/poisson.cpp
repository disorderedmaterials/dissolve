// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/poissonFit.h"
#include "nodes/importXYData.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
void testReconstruction(std::string inpAFile, std::string delfitFile,
                        const std::vector<std::tuple<std::string, int, double>> &targets, Data1D axisData)
{
    LineParser parser;
    ASSERT_TRUE(parser.openInput(inpAFile));

    // First line contains parameters relating to energy / pressure calculation which we can skip
    ASSERT_TRUE(parser.getArgsDelim(LineParser::Defaults) == LineParser::Success);

    for (auto &[dataSet, column, errorThreshold] : targets)
    {
        // Data file index
        ASSERT_TRUE(parser.getArgsDelim(LineParser::Defaults) == LineParser::Success);

        std::cout << std::format("Reading fit coefficients for data {} ({})...\n", dataSet, parser.argi(0));

        // Number of coefficients, stepsize in r, sigma2
        ASSERT_TRUE(parser.getArgsDelim(LineParser::Defaults) == LineParser::Success);
        const auto nCoeff = parser.argi(0);

        // Coefficients (all on one line)
        ASSERT_TRUE(parser.getArgsDelim(LineParser::Defaults) == LineParser::Success);
        ASSERT_TRUE(parser.nArgs() == nCoeff);

        std::vector<double> fitCoefficients;
        fitCoefficients.resize(nCoeff);
        for (auto i = 0; i < nCoeff; ++i)
            fitCoefficients[i] = parser.argd(i);

        // Construct the fit
        PoissonFit coeffMinimiser(axisData);
        coeffMinimiser.constructReciprocal(0.0, 12.0, fitCoefficients, 0.01, 0.01, 0, 0.01, 0);

        EXPECT_TRUE(checkData1D(coeffMinimiser.approximation(), dataSet, delfitFile, 1, column, errorThreshold));
    }
}

TEST(Poisson, WaterInpA)
{
    // Prepare a dummy dataset with the correct x axis for the Poisson reconstruction
    Data1D dummyData;
    ASSERT_TRUE(ImportXYDataNode::read(dummyData, "epsr25/water1000-neutron/FQ.delfit", 1, 2, 0, true));
    dummyData.zero();

    // Set targets
    const std::vector<std::tuple<std::string, int, double>> targets = {
        {"D2O", 2, 8.0e-4}, {"H2O", 4, 7.0e-4}, {"HDO", 6, 4.0e-4}};

    testReconstruction("epsr25/water1000-neutron/water.EPSR.inpa", "epsr25/water1000-neutron/FQ.delfit", targets, dummyData);
}

TEST(Poisson, BenzeneInpA)
{
    // Prepare a dummy dataset with the correct x axis for the Poisson reconstruction
    Data1D dummyData;
    ASSERT_TRUE(ImportXYDataNode::read(dummyData, "epsr25/benzene200-neutron/FQ.delfit", 1, 2, 0, true));
    dummyData.zero();

    // Set targets
    std::vector<std::tuple<std::string, int, double>> targets = {{"C6H6", 2, 2.5e-4}, {"C6D6", 4, 5.0e-4}, {"5050", 6, 4.0e-4}};

    testReconstruction("epsr25/benzene200-neutron/benzene.EPSR.inpa", "epsr25/benzene200-neutron/FQ.delfit", targets,
                       dummyData);
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/applicative.h"
#include "base/parserLibrary.h"
#include "math/poissonFit.h"
#include "nodes/importXYData.h"
#include "tests/testing.h"
#include <fstream>

namespace UnitTest
{
void testReconstruction(std::string inpAFile, std::string delfitFile,
                        const std::vector<std::tuple<std::string, int, double>> &targets, Data1D axisData)
{
    using namespace Parsers;
    std::ifstream infile(inpAFile);
    ASSERT_TRUE(infile);

    // First line contains parameters relating to energy / pressure calculation which we can skip
    ASSERT_TRUE(toEndOfLine().parse(infile));

    for (auto &[dataSet, column, errorThreshold] : targets)
    {
        // Data file index
        auto index = (inlineSpaces() >> natural() << toEndOfLine()).parse(infile);
        ASSERT_TRUE(index);

        std::cout << std::format("Reading fit coefficients for data {} ({})...\n", dataSet, std::get<0>(*index));

        // Coefficients (all on one line)
        auto count = (inlineSpaces() >> natural() << toEndOfLine()).parse(infile);
        ASSERT_TRUE(count);
        auto nCoeff = std::get<0>(*count);
        auto coefficients = (some(inlineSpaces() >> real()) << newlines()).parse(infile);
        ASSERT_TRUE(coefficients);
        auto fitCoefficients = std::get<0>(*coefficients);
        ASSERT_EQ(nCoeff, fitCoefficients.size());

        // Construct the fit
        PoissonFit coeffMinimiser(axisData);
        coeffMinimiser.constructReciprocal(0.0, 12.0, fitCoefficients, 0.01, 0.01, 0, 0.01, 0);

        EXPECT_TRUE(testData1D(coeffMinimiser.approximation(), dataSet, delfitFile, 1, column, errorThreshold));
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

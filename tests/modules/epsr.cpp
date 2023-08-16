// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/epsr/epsr.h"
#include "classes/configuration.h"
#include "keywords/double.h"
#include "main/dissolve.h"
#include <gmock/gmock.h>
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

TEST(EPSRModuleTest, ScatteringMatrix)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/epsr-5datasets.txt"));
    dissolve.prepare();
    ASSERT_TRUE(dissolve.iterate(1));

    // Find EPSR module
    auto modules = Module::allOfType<EPSRModule>();
    EXPECT_EQ(modules.size(), 1);
    auto *epsrModule = modules.front();

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

TEST(EPSRModuleTest, DataWeighting)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("dissolve/epsr-5datasets-weighted.txt"));
    dissolve.prepare();
    ASSERT_TRUE(dissolve.iterate(1));

    // Find EPSR module
    auto modules = Module::allOfType<EPSRModule>();
    EXPECT_EQ(modules.size(), 1);
    auto *epsrModule = modules.front();

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

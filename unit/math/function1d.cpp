// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/function1D.h"
#include "math/data1D.h"
#include "templates/algorithms.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
constexpr double binWidth = 0.01;

class Function1DTest : public ::testing::Test
{
    public:
    Function1DTest() = default;

    Data1D generate(Functions::Function1DWrapper f, double omega = 0.0, double xBegin = -5.0, double xEnd = 5.0,
                    double xDelta = binWidth)
    {
        Data1D d;
        auto x = xBegin;
        while (x <= xEnd)
        {
            d.addPoint(x, f.y(x, omega));
            x += xDelta;
        }
        return d;
    }
};

TEST_F(Function1DTest, Basic)
{
    // Basic Gaussian, testing class behaviour
    auto basicGaussian = generate(Functions::Function1DWrapper(Functions::Function1D::Gaussian, {0.2}));
    auto c = 0.2 / (2.0 * sqrt(2.0 * log(2.0)));
    for (auto &&[x, y] : zip(basicGaussian.xAxis(), basicGaussian.values()))
        EXPECT_NEAR(y, exp(-(x * x) / (2 * c * c)), 1.0e-10);
}

TEST_F(Function1DTest, UnitIntegral)
{
    // Some random omega values for Q-dependency testing
    std::vector<double> omegas = {0.189, 0.78, 1.102, 3.22, 4.8};

    Functions::Function1DWrapper f;
    Data1D xy;

    // Gaussian
    f.setFunctionAndParameters(Functions::Function1D::Gaussian, {0.2});
    xy = generate(f);
    EXPECT_NEAR(1.0 / binWidth, f.normalisation() * std::accumulate(xy.values().begin(), xy.values().end(), 0.0), 1.0e-10);

    // Scaled Gaussian
    f.setFunctionAndParameters(Functions::Function1D::ScaledGaussian, {4.325215, 0.38});
    xy = generate(f);
    EXPECT_NEAR(1.0 / binWidth, f.normalisation() * std::accumulate(xy.values().begin(), xy.values().end(), 0.0), 1.0e-10);

    // Omega-Dependent Gaussian
    f.setFunctionAndParameters(Functions::Function1D::OmegaDependentGaussian, {0.189});
    for (auto omega : omegas)
    {
        xy = generate(f, omega);
        EXPECT_NEAR(1.0 / binWidth, f.normalisation(omega) * std::accumulate(xy.values().begin(), xy.values().end(), 0.0),
                    1.0e-10);
    }
    f.setParameters({0.254});
    for (auto omega : omegas)
    {
        xy = generate(f, omega);
        EXPECT_NEAR(1.0 / binWidth, f.normalisation(omega) * std::accumulate(xy.values().begin(), xy.values().end(), 0.0),
                    1.0e-10);
    }

    // Scaled Gaussian
    f.setFunctionAndParameters(Functions::Function1D::GaussianC2, {0.1, 0.3});
    for (auto omega : omegas)
    {
        xy = generate(f, omega);
        EXPECT_NEAR(1.0 / binWidth, f.normalisation(omega) * std::accumulate(xy.values().begin(), xy.values().end(), 0.0),
                    1.0e-10);
    }
}

} // namespace UnitTest

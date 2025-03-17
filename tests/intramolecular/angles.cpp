// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/speciesAngle.h"
#include "math/data1D.h"
#include "math/derivative.h"
#include "math/interpolator.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class AngleFunctionsAnalyticTest : public ::testing::Test
{
    public:
    AngleFunctionsAnalyticTest() {}

    protected:
    // Angle
    SpeciesAngle ijk_;
    // Constants
    const double rangeMin_{90.0}, rangeMax_{110.0}, delta_{0.0001};
    static constexpr double testDelta_{0.214};

    public:
    // Test angle function for consistency between energy and (derived) force and analytic force
    void test(AngleFunctions::Form form, std::string parameters)
    {
        // Set the angle potential parameters
        ijk_.setInteractionForm(form);
        ASSERT_TRUE(ijk_.setInteractionParameters(parameters));

        // Construct tabulated energy and force
        Data1D energy;
        auto x = rangeMin_;
        while (x <= rangeMax_)
        {
            energy.addPoint(x / DEGRAD, ijk_.energy(x));
            x += delta_;
        }
        auto de_dx = Derivative::derivative(energy);
        de_dx *= -1.0;
        Interpolator force(de_dx);
        force.interpolate();

        // Test analytic vs tabulated values - do this by absolute value if less than 1.0, or by ratio if greater than 1.0.
        // We do this since we span many orders of magnitude in value over the potential range, and can also have zeroes.
        auto theta = rangeMin_ + testDelta_;
        while (theta < rangeMax_)
        {
            auto f = ijk_.force(theta);
            if (fabs(f) < 1.0)
                EXPECT_NEAR(f, force.y(theta / DEGRAD) / -sin(theta / DEGRAD), 1.0e-5);
            else
                EXPECT_NEAR(f / (force.y(theta / DEGRAD) / -sin(theta / DEGRAD)), 1.0, 1.0e-3);
            theta += testDelta_;
        }
    }
};

TEST_F(AngleFunctionsAnalyticTest, NoneForm) { test(AngleFunctions::Form::None, ""); }

TEST_F(AngleFunctionsAnalyticTest, HarmonicForm)
{
    test(AngleFunctions::Form::Harmonic, "k=200.2 eq=100.0");
    test(AngleFunctions::Form::Harmonic, "k=187.2 eq=97.35");
}

TEST_F(AngleFunctionsAnalyticTest, CosineForm) { test(AngleFunctions::Form::Cosine, "k=100.0 n=3.0 eq=100.0 s=1.0"); }

TEST_F(AngleFunctionsAnalyticTest, Cos2Form) { test(AngleFunctions::Form::Cos2, "k=55.0 c0=1.0 c1=2.45 c2=0.0"); }
} // namespace UnitTest

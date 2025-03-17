// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/speciesBond.h"
#include "math/data1D.h"
#include "math/derivative.h"
#include "math/interpolator.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class BondFunctionsAnalyticTest : public ::testing::Test
{
    public:
    BondFunctionsAnalyticTest() {}

    protected:
    // Bond
    SpeciesBond ij_;
    // Constants
    const double rangeMin_{1.0}, rangeMax_{2.0}, delta_{0.0001};
    static constexpr double testRDelta_{0.002088};

    public:
    // Test bond function for consistency between energy and (derived) force and analytic force
    void test(BondFunctions::Form form, std::string parameters)
    {
        // Set the bond potential parameters
        ij_.setInteractionForm(form);
        ASSERT_TRUE(ij_.setInteractionParameters(parameters));

        // Construct tabulated energy and force
        Data1D energy;
        auto x = rangeMin_;
        while (x <= rangeMax_)
        {
            energy.addPoint(x, ij_.energy(x));
            x += delta_;
        }
        auto de_dx = Derivative::derivative(energy);
        de_dx *= -1.0;
        Interpolator force(de_dx);
        force.interpolate();

        // Test analytic vs tabulated values - do this by absolute value if less than 1.0, or by ratio if greater than 1.0.
        // We do this since we span many orders of magnitude in value over the potential range, and can also have zeroes.
        auto r = rangeMin_ + testRDelta_;
        while (r < rangeMax_)
        {
            auto f = ij_.force(r);
            if (fabs(f) < 1.0)
                EXPECT_NEAR(f, force.y(r), 1.0e-5);
            else
                EXPECT_NEAR(f / force.y(r), 1.0, 1.0e-3);
            r += testRDelta_;
        }
    }
};

TEST_F(BondFunctionsAnalyticTest, NoneForm) { test(BondFunctions::Form::None, ""); }

TEST_F(BondFunctionsAnalyticTest, HarmonicForm)
{
    test(BondFunctions::Form::Harmonic, "k=2045.2 eq=1.5");
    test(BondFunctions::Form::Harmonic, "k=2045.2 eq=1.2817");
}

TEST_F(BondFunctionsAnalyticTest, MorseForm)
{
    test(BondFunctions::Form::Morse, "D=1042.01 alpha=2.0 eq=1.5");
    test(BondFunctions::Form::Morse, "D=970.0 alpha=0.1 eq=1.624");
}

TEST_F(BondFunctionsAnalyticTest, BuckinghamForm) { test(BondFunctions::Form::Buckingham, "A=12345.1 B=3.3 C=1000.0"); }
} // namespace UnitTest

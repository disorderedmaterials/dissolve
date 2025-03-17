// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/speciesAtom.h"
#include "classes/speciesTorsion.h"
#include "math/data1D.h"
#include "math/derivative.h"
#include "math/interpolator.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class TorsionFunctionsAnalyticTest : public ::testing::Test
{
    public:
    TorsionFunctionsAnalyticTest() {}

    protected:
    // Torsion
    SpeciesTorsion ijkl_;
    // Constants
    const double rangeMin_{-100.0}, rangeMax_{20.0}, delta_{0.001};
    static constexpr double testDelta_{0.214};

    public:
    // Test angle function for consistency between energy and (derived) force and analytic force
    void test(TorsionFunctions::Form form, std::string parameters)
    {
        // Set the angle potential parameters
        ijkl_.setInteractionForm(form);
        ASSERT_TRUE(ijkl_.setInteractionParameters(parameters));

        // Construct tabulated energy and force
        Data1D energy;
        auto x = rangeMin_;
        while (x <= rangeMax_)
        {
            energy.addPoint(x / DEGRAD, ijkl_.energy(x));
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
            auto f = ijkl_.force(theta);
            if (fabs(f) < 1.0)
                EXPECT_NEAR(f, force.y(theta / DEGRAD) / -sin(theta / DEGRAD), 1.0e-5);
            else
                EXPECT_NEAR(f / (force.y(theta / DEGRAD) / -sin(theta / DEGRAD)), 1.0, 1.0e-3);
            theta += testDelta_;
        }
    }
};

TEST_F(TorsionFunctionsAnalyticTest, NoneForm) { test(TorsionFunctions::Form::None, ""); }

TEST_F(TorsionFunctionsAnalyticTest, CosineForm)
{
    test(TorsionFunctions::Form::Cosine, "k=20.2 n=3 eq=109.0 s=1.0");
    test(TorsionFunctions::Form::Cosine, "k=18.2 n=2 eq=45.0 s=-1");
}

TEST_F(TorsionFunctionsAnalyticTest, Cos3Form)
{
    test(TorsionFunctions::Form::Cos3, "k1=1.0 k2=3.0 k3=-0.5");
    test(TorsionFunctions::Form::Cos3, "k1=0.0 k2=4.5 k3=0.0");
}

TEST_F(TorsionFunctionsAnalyticTest, Cos3CForm)
{
    test(TorsionFunctions::Form::Cos3C, "k0=2.0 k1=-0.6 k2=2.0 k3=-0.5");
    test(TorsionFunctions::Form::Cos3C, "k0=0.2 k1=6.0 k2=-2.0 k3=0.0");
}

TEST_F(TorsionFunctionsAnalyticTest, Cos4Form)
{
    test(TorsionFunctions::Form::Cos4, "k1=1.0 k2=3.0 k3=-0.5 k4=-2.0");
    test(TorsionFunctions::Form::Cos4, "k1=0.0 k2=4.5 k3=0.0 k4=4.0");
}

TEST_F(TorsionFunctionsAnalyticTest, CosNForm)
{
    test(TorsionFunctions::Form::Cos3, "k1=1.0 k2=3.0 k3=-0.5");
    test(TorsionFunctions::Form::CosN, "k1=0.0 k2=4.5 k3=0.0");
    test(TorsionFunctions::Form::CosN, "k1=1.0 k2=3.0 k3=-0.5 k4=-2.0");
    test(TorsionFunctions::Form::CosN, "k1=0.0 k2=4.5 k3=0.0 k4=4.0");
}

TEST_F(TorsionFunctionsAnalyticTest, CosNCForm)
{
    test(TorsionFunctions::Form::CosNC, "k0=2.0 k1=-0.6 k2=2.0 k3=-0.5");
    test(TorsionFunctions::Form::CosNC, "k0=0.2 k1=6.0 k2=-2.0 k3=0.0");
    test(TorsionFunctions::Form::CosNC, "k0=2.0 k1=-0.6 k2=2.0 k3=-0.5");
    test(TorsionFunctions::Form::CosNC, "k0=0.2 k1=6.0 k2=-2.0 k3=0.0");
}

TEST_F(TorsionFunctionsAnalyticTest, UFFCosineForm)
{
    test(TorsionFunctions::Form::UFFCosine, "k=20.2 n=3 eq=109.0");
    test(TorsionFunctions::Form::UFFCosine, "k=18.2 n=2 eq=45.0");
}

TEST_F(TorsionFunctionsAnalyticTest, FourierForm)
{
    test(TorsionFunctions::Form::FourierN, "k=1.234 C0=1.0 C1=-2.0 C2=4.0 C3=0.8");
    test(TorsionFunctions::Form::FourierN, "k=4.321 C0=0.3 C1=1.89 C2=-1.442");
}

} // namespace UnitTest

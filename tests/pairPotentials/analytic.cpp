// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/pairPotential.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class PairPotentialsAnalyticTest : public ::testing::Test
{
    public:
    PairPotentialsAnalyticTest()
    {
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);

        A_ = std::make_shared<AtomType>(Elements::H);
        A_->setName("A");
        A_->interactionPotential().setForm(ShortRangeFunctions::Form::Undefined);
        B_ = std::make_shared<AtomType>(Elements::H);
        B_->setName("B");
        B_->interactionPotential().setForm(ShortRangeFunctions::Form::Undefined);

        pairPotential_ = std::make_shared<PairPotential>("A", "B");
    }

    protected:
    // AtomTypes
    std::shared_ptr<AtomType> A_, B_;
    // PairPotential
    std::shared_ptr<PairPotential> pairPotential_;
    // Constants
    const double ppRange_{15.0};
    const double ppDelta_{0.0001};
    static constexpr double testRDelta_{0.00173};

    private:
    void test(Functions1D::Form form, std::string_view parameters, double rStart, const std::function<double(double)> &analytic,
              const std::function<double(double)> &tabulated)
    {
        // Set up and tabulate pair potential
        pairPotential_->setInteractionPotential(form, parameters);
        pairPotential_->tabulate(ppRange_, ppDelta_);

        // Test analytic vs tabulated values - do this by absolute value if less than 1.0, or by ratio if greater than 1.0.
        // We do this since we span many orders of magnitude in value over the potential range, and can also have zeroes.
        auto r = rStart;
        while (r < ppRange_)
        {
            auto a = analytic(r);
            if (fabs(a) < 1.0)
                EXPECT_NEAR(a, tabulated(r), 1.0e-5);
            else
                EXPECT_NEAR(a / tabulated(r), 1.0, 1.0e-3);
            r += testRDelta_;
        }
    }

    public:
    // Test analytic vs tabulated energy for specified form and parameters
    void testEnergy(Functions1D::Form form, std::string_view parameters, double rStart = testRDelta_)
    {
        test(
            form, parameters, rStart, [=](double r) { return pairPotential_->analyticEnergy(r, 1.0, 1.0); },
            [=](double r) { return pairPotential_->energy(r); });
    }
    // Test analytic vs tabulated force for specified form and parameters
    void testForce(Functions1D::Form form, std::string_view parameters, double rStart = testRDelta_)
    {
        test(
            form, parameters, rStart, [=](double r) { return pairPotential_->analyticForce(r, 1.0, 1.0); },
            [=](double r) { return pairPotential_->force(r); });
    }
};

TEST_F(PairPotentialsAnalyticTest, NoneForm)
{
    testEnergy(Functions1D::Form::None, "");
    testForce(Functions1D::Form::None, "");
}

TEST_F(PairPotentialsAnalyticTest, LennardJonesForm)
{
    // Lennard-Jones is super steep at r -> 0 so start a little after that
    testEnergy(Functions1D::Form::LennardJones126, "epsilon=0.35 sigma=2.166", testRDelta_ * 5);
    testForce(Functions1D::Form::LennardJones126, "epsilon=0.35 sigma=2.166", testRDelta_ * 5);
}

TEST_F(PairPotentialsAnalyticTest, Buckingham)
{
    // Values put in for TeO2 from https://pubs.rsc.org/en/content/articlelanding/2014/cp/c4cp01273a
    testEnergy(Functions1D::Form::Buckingham, "A=153919.5503  B=2.8912848  C=96.48514925", testRDelta_ * 5);
    testForce(Functions1D::Form::Buckingham, "A=8005439.257  B=6.211565  C=3025.962812", testRDelta_ * 5);
}

TEST_F(PairPotentialsAnalyticTest, Gaussian)
{
    testEnergy(Functions1D::Form::GaussianPotential, "A=1.0  fwhm=2.4  x0=0", testRDelta_ * 5);
    testForce(Functions1D::Form::GaussianPotential, "A=1.0  fwhm=2.4  x0=0", testRDelta_ * 5);

    testEnergy(Functions1D::Form::GaussianPotential, "A=10.0  fwhm=1.1  x0=5.0", testRDelta_ * 5);
    testForce(Functions1D::Form::GaussianPotential, "A=10.0  fwhm=1.1  x0=5.0", testRDelta_ * 5);
}
} // namespace UnitTest

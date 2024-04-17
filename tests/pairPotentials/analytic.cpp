// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/pairPotential.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class PairPotentialsTest : public ::testing::Test
{
    public:
    PairPotentialsTest()
    {
        PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);

        A_ = std::make_shared<AtomType>(Elements::H);
        A_->setName("A");
        A_->interactionPotential().setForm(ShortRangeFunctions::Form::None);
        B_ = std::make_shared<AtomType>(Elements::H);
        B_->setName("B");
        B_->interactionPotential().setForm(ShortRangeFunctions::Form::None);

        pairPotential_ = std::make_shared<PairPotential>(A_, B_, false);
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
    void test(ShortRangeFunctions::Form form, std::string_view parameters, double rStart,
              const std::function<double(double)> &analytic, const std::function<double(double)> &tabulated)
    {
        // Set up and tabulate pair potential
        pairPotential_->interactionPotential().setFormAndParameters(form, parameters);
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
    void testEnergy(ShortRangeFunctions::Form form, std::string_view parameters, double rStart = testRDelta_)
    {
        test(
            form, parameters, rStart, [=](double r) { return pairPotential_->analyticEnergy(r); },
            [=](double r) { return pairPotential_->energy(r); });
    }
    // Test analytic vs tabulated force for specified form and parameters
    void testForce(ShortRangeFunctions::Form form, std::string_view parameters, double rStart = testRDelta_)
    {
        test(
            form, parameters, rStart, [=](double r) { return pairPotential_->analyticForce(r); },
            [=](double r) { return pairPotential_->force(r); });
    }
};

TEST_F(PairPotentialsTest, NoneForm)
{
    testEnergy(ShortRangeFunctions::Form::None, "");
    testForce(ShortRangeFunctions::Form::None, "");
}

TEST_F(PairPotentialsTest, LennardJonesForm)
{
    // Lennard-Jones is super steep at r -> 0 so start a little after that
    testEnergy(ShortRangeFunctions::Form::LennardJones, "epsilon=0.35 sigma=2.166", testRDelta_ * 5);
    testForce(ShortRangeFunctions::Form::LennardJones, "epsilon=0.35 sigma=2.166", testRDelta_ * 5);
}

} // namespace UnitTest
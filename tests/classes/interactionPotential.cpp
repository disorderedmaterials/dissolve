// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/speciesTorsion.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(InteractionPotentialTest, ParameterNames)
{
    InteractionPotential<ShortRangeFunctions> srPotential(ShortRangeFunctions::Form::LennardJones);
    auto epsilon = 1.1, sigma = 2.34;

    // Valid parameters
    EXPECT_TRUE(srPotential.parseParameters(fmt::format("epsilon={} sigma={}", epsilon, sigma)));
    EXPECT_EQ(srPotential.nParameters(), 2);
    EXPECT_DOUBLE_EQ(srPotential.parameters()[0], epsilon);
    EXPECT_DOUBLE_EQ(srPotential.parameters()[1], sigma);

    // Valid parameters, switched order
    EXPECT_TRUE(srPotential.parseParameters(fmt::format("sigma={} epsilon={}", sigma, epsilon)));
    EXPECT_EQ(srPotential.nParameters(), 2);
    EXPECT_DOUBLE_EQ(srPotential.parameters()[0], epsilon);
    EXPECT_DOUBLE_EQ(srPotential.parameters()[1], sigma);

    // Wrong number of parameters
    EXPECT_FALSE(srPotential.parseParameters(fmt::format("sigma={} epsilon={} suspect={}", sigma, epsilon, 1.0)));

    // Invalid parameter
    EXPECT_FALSE(srPotential.parseParameters(fmt::format("sigma={} suspect={}", sigma, epsilon)));
}

TEST(InteractionPotentialTest, DynamicParameterNames1)
{
    InteractionPotential<TorsionFunctions> torsionPotential(TorsionFunctions::Form::CosN);

    // Ordered parameters (k1, k2, etc.)
    EXPECT_TRUE(torsionPotential.parseParameters(
        joinStrings(std::vector<int>{1, 2, 3, 4, 5}, " ", [](const auto n) { return fmt::format("k{}={}", n, n); })));
    EXPECT_EQ(torsionPotential.nParameters(), 5);
    EXPECT_EQ(torsionPotential.parametersAsString(), "k1=1 k2=2 k3=3 k4=4 k5=5");
    for (auto n = 0; n < 5; ++n)
        EXPECT_DOUBLE_EQ(torsionPotential.parameters()[n], n + 1);

    // Disordered parameters (k5, k2, etc.)
    EXPECT_TRUE(torsionPotential.parseParameters(
        joinStrings(std::vector<int>{5, 2, 1, 3, 4}, " ", [](const auto n) { return fmt::format("k{}={}", n, n); })));
    EXPECT_EQ(torsionPotential.nParameters(), 5);
    EXPECT_EQ(torsionPotential.parametersAsString(), "k1=1 k2=2 k3=3 k4=4 k5=5");
    for (auto n = 0; n < 5; ++n)
        EXPECT_DOUBLE_EQ(torsionPotential.parameters()[n], n + 1);
}

} // namespace UnitTest

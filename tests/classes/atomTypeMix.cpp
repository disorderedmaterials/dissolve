// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/configuration.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(AtomTypeMixTest, Basic)
{
    SmallMolecules molecules_;
    AtomTypeMix mix_;

    // Add atom types to our mix
    mix_.add(molecules_.atN().get(), Sears91::N_Natural, 1);
    mix_.add(molecules_.atOW().get(), Sears91::O_Natural, 2);
    mix_.add(molecules_.atHW().get(), Sears91::H_Natural, 3);
    mix_.finalise({});

    // Check basic data
    EXPECT_EQ(mix_.mix().size(), 3);
    EXPECT_TRUE(mix_.mix().contains(molecules_.atN().get()));
    EXPECT_TRUE(mix_.mix().contains(molecules_.atHW().get()));
    EXPECT_TRUE(mix_.mix().contains(molecules_.atOW().get()));
    EXPECT_FALSE(mix_.mix().contains(molecules_.atH1().get()));

    // Check detailed data
    EXPECT_DOUBLE_EQ(mix_.mix().get(molecules_.atN().get())->fraction(), 1.0 / 6.0);
    EXPECT_DOUBLE_EQ(mix_.mix().get(molecules_.atOW().get())->fraction(), 1.0 / 3.0);
    EXPECT_DOUBLE_EQ(mix_.mix().get(molecules_.atHW().get())->fraction(), 1.0 / 2.0);
}

} // namespace UnitTest

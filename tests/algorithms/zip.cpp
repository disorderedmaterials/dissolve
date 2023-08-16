// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "templates/algorithms.h"
#include <array>
#include <gtest/gtest.h>
#include <string_view>
#include <vector>

namespace UnitTest
{

TEST(ZipTest, PythagoreanTriples)
{
    std::vector<double> as = {3, 5, 30};
    std::array<double, 3> bs = {4, 12, 40};
    std::array<double, 3> cs;

    int count = 0;
    for (auto &&[a, b, c] : zip(as, bs, cs))
    {
        c = a * a + b * b;
        ++count;
    }
    EXPECT_EQ(count, as.size());
    EXPECT_EQ(count, bs.size());
    EXPECT_EQ(count, cs.size());
    for (int n = 0; n < as.size(); ++n)
    {
        EXPECT_EQ(as[n] * as[n] + bs[n] * bs[n], cs[n]);
    }
}

} // namespace UnitTest

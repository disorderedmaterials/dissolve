// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/elements.h"
#include <gtest/gtest.h>

TEST(ElementsTest, ElementZ)
{
    // Ensure
    for (auto n = 0; n < Elements::nElements; ++n)
        ASSERT_EQ(n, Elements::element(n));
}

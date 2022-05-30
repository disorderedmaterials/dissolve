// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(TOMLTest, Parse)
{
    if constexpr (Dissolve::toml_testing_flag)
    {
        std::vector<std::string> inputs = {"inputs/benzene.txt", "inputs/water.txt", "inputs/py5-ntf2.txt"};

        for (auto &input : inputs)
        {
            CoreData coreData, coreData2;
            Dissolve initial(coreData);
            initial.loadInput(input);
            auto toml = initial.serialise();
            Dissolve repeat(coreData2);
            EXPECT_NO_THROW(repeat.deserialise(toml));
            auto toml2 = repeat.serialise();

            EXPECT_EQ(toml, toml2);
        }
    }
}

} // namespace UnitTest

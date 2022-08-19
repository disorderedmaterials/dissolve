// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{

void runParse(std::string input)
{
    if constexpr (Dissolve::toml_testing_flag)
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

TEST(TOMLTest, Parse_Inputs_Benzene) { runParse("inputs/benzene"); }
TEST(TOMLTest, Parse_Inputs_Water) { runParse("inputs/water"); }
TEST(TOMLTest, Parse_Inputs_Py5_Ntf2) { runParse("inputs/py5-ntf2"); }
TEST(TOMLTest, Parse_Bragg_Intensities) { runParse("bragg/intensities"); }

} // namespace UnitTest

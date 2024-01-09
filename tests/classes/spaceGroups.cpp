// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/spaceGroups.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SpaceGroupsTest, HallSymbols)
{
    // Search for valid symbols
    std::vector<std::pair<std::string, SpaceGroups::SpaceGroupId>> validSymbols = {
        {{"-I 2 2c", SpaceGroups::SpaceGroup_72},
         {"-I 2a 2b", SpaceGroups::SpaceGroup_73_baC},
         {"-B 2xbc", SpaceGroups::SpaceGroup_15_A2},
         {"-C 2xbc", SpaceGroups::SpaceGroup_15_a2},
         {"-P 3 2\"c", SpaceGroups::SpaceGroup_165},
         {"I 4bw 2bw -1bw", SpaceGroups::SpaceGroup_141_1}}};
    for (const auto &[name, symbol] : validSymbols)
        EXPECT_EQ(SpaceGroups::findByHallSymbol(name), symbol);

    // Non-existent symbols
    EXPECT_EQ(SpaceGroups::findByHallSymbol(""), SpaceGroups::NoSpaceGroup);
    EXPECT_EQ(SpaceGroups::findByHallSymbol("Beans"), SpaceGroups::NoSpaceGroup);
    EXPECT_EQ(SpaceGroups::findByHallSymbol("P n/8 x y"), SpaceGroups::NoSpaceGroup);
}

TEST(SpaceGroupsTest, HermannMauginnSymbols)
{
    // Search for valid symbols
    std::vector<std::tuple<std::string, std::string, SpaceGroups::SpaceGroupId>> validSymbols = {
        {{"I b a m", "", SpaceGroups::SpaceGroup_72},
         {"I b c a", "", SpaceGroups::SpaceGroup_73},
         {"I c a b", "", SpaceGroups::SpaceGroup_73_baC},
         {"B 2/n 1 1", "", SpaceGroups::SpaceGroup_15_A2},
         {"C 2/n 1 1", "", SpaceGroups::SpaceGroup_15_a2},
         {"B b c b:1", "", SpaceGroups::SpaceGroup_68_1bca},
         {"F d -3 c:1", "", SpaceGroups::SpaceGroup_228_1},
         {"F d -3 c", "", SpaceGroups::SpaceGroup_228_1},
         {"F d -3 c:1", "", SpaceGroups::SpaceGroup_228_1},
         {"F d -3 c", "1", SpaceGroups::SpaceGroup_228_1},
         {"F d -3 c:2", "", SpaceGroups::SpaceGroup_228_2},
         {"F d -3 c", "2", SpaceGroups::SpaceGroup_228_2}}};
    for (const auto &[name, code, symbol] : validSymbols)
    {
        fmt::print("We are testing {} {}\n", name, code);
        EXPECT_EQ(SpaceGroups::findByHermannMauginnSymbol(name, code), symbol);
    }

    // Non-existent symbols
    EXPECT_EQ(SpaceGroups::findByHermannMauginnSymbol(""), SpaceGroups::NoSpaceGroup);
    EXPECT_EQ(SpaceGroups::findByHermannMauginnSymbol("Beans"), SpaceGroups::NoSpaceGroup);
    EXPECT_EQ(SpaceGroups::findByHermannMauginnSymbol("F d -3 c", "4"), SpaceGroups::NoSpaceGroup);
    EXPECT_EQ(SpaceGroups::findByHermannMauginnSymbol("F d -3 c:3"), SpaceGroups::NoSpaceGroup);
}
} // namespace UnitTest

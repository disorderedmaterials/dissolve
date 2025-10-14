// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/sysFunc.h"
#include <gtest/gtest.h>
#include <string_view>

namespace UnitTest
{
struct NameObject
{
    NameObject(std::string newName) : name(std::move(newName)) {};
    std::string name;
};

TEST(SysFunc, NiceName)
{
    // Nice name
    EXPECT_TRUE(DissolveSys::niceName(" /\\#*$") == "______");
    EXPECT_TRUE(DissolveSys::niceName("Under score") == "Under_score");
}

TEST(SysFunc, StringMatchingSimple)
{
    // String matching
    EXPECT_TRUE(DissolveSys::sameString("Hello", "Hello", true));
    EXPECT_TRUE(DissolveSys::sameString("Hello", "hello"));
    EXPECT_FALSE(DissolveSys::sameString("Hello", "hello", true));
}

TEST(SysFunc, StringMatchingWildcard)
{
    // Wildcard string matching
    EXPECT_TRUE(DissolveSys::sameWildString("*", "Literally Anything"));
    EXPECT_FALSE(DissolveSys::sameWildString("*A", "Not Quite Anything"));
    EXPECT_TRUE(DissolveSys::sameWildString("*g", "Not Quite Anything"));
    EXPECT_TRUE(DissolveSys::sameWildString("Not*g", "Not Quite Anything"));
    EXPECT_TRUE(DissolveSys::sameWildString("Not***Any*g", "Not Quite Anything"));
    EXPECT_FALSE(DissolveSys::sameWildString("ot***Any*g", "Not Quite Anything"));
    EXPECT_TRUE(DissolveSys::sameWildString("?ot***Any*g", "Not Quite Anything"));
    EXPECT_TRUE(DissolveSys::sameWildString("abc*efg", "ABCDEFG"));
    EXPECT_FALSE(DissolveSys::sameWildString("abc*efg", "ABCDEFG", true));
    EXPECT_TRUE(DissolveSys::sameWildString("abc*efg", "ABCDEFG"));
    EXPECT_FALSE(DissolveSys::sameWildString("abc*efg", "ABCDEFGH"));
    EXPECT_TRUE(DissolveSys::sameWildString("abc*efg?", "ABCDEFGH"));
}

TEST(SysFunc, StringBeforeAfter)
{
    // Before / after chars and strings
    EXPECT_TRUE(DissolveSys::beforeChar("Something=This", '=') == "Something");
    EXPECT_TRUE(DissolveSys::afterChar("Something=This", '=') == "This");
    EXPECT_TRUE(DissolveSys::beforeChar("Something=This=That=Other", '=') == "Something");
    EXPECT_TRUE(DissolveSys::afterChar("Something=This=That=Other", '=') == "This=That=Other");
    EXPECT_TRUE(DissolveSys::afterString("Something=This=That=Other", "This=") == "That=Other");
    EXPECT_TRUE(DissolveSys::beforeLastChar("Something=This=That=Other", '=') == "Something=This=That");
    EXPECT_TRUE(DissolveSys::afterLastChar("Something=This=That=Other", '=') == "Other");
    EXPECT_TRUE(DissolveSys::beforeChar("Just a string", '=').empty());
    EXPECT_TRUE(DissolveSys::afterChar("Just a string", '=').empty());
    EXPECT_TRUE(DissolveSys::beforeLastChar("Just a string", '=').empty());
    EXPECT_TRUE(DissolveSys::afterLastChar("Just a string", '=').empty());
    EXPECT_TRUE(DissolveSys::afterString("Just a string", "I'm not here").empty());
    EXPECT_TRUE(DissolveSys::afterString("Just a string", "string").empty());
}

TEST(SysFunc, StringStartsEnds)
{
    // Starts / ends with
    EXPECT_TRUE(DissolveSys::startsWith("I am a little man", "I am"));
    EXPECT_FALSE(DissolveSys::startsWith("I am a little man", "i am"));
    EXPECT_TRUE(DissolveSys::endsWith("I have a plan", "plan"));
    EXPECT_FALSE(DissolveSys::endsWith("I have a plan", "man"));
}

TEST(SysFunc, StringToNumber)
{
    // Conversion to numbers
    EXPECT_TRUE(DissolveSys::isNumber("1.0"));
    EXPECT_TRUE(DissolveSys::isNumber("1.0E-5"));
    EXPECT_TRUE(DissolveSys::isNumber("+5.712945e+4"));
    EXPECT_TRUE(DissolveSys::isNumber("100"));
    EXPECT_FALSE(DissolveSys::isNumber("abc"));
    EXPECT_FALSE(DissolveSys::isNumber("."));
    EXPECT_FALSE(DissolveSys::isNumber(""));
    EXPECT_FALSE(DissolveSys::isNumber("e-1"));

    // Conversion to numbers and type detection
    bool isFP = false;
    EXPECT_TRUE(DissolveSys::isNumber("1.0", isFP));
    EXPECT_TRUE(isFP);
    EXPECT_TRUE(DissolveSys::isNumber("1e-5", isFP));
    EXPECT_TRUE(isFP);
    EXPECT_TRUE(DissolveSys::isNumber("1e5", isFP));
    EXPECT_FALSE(isFP);
    EXPECT_TRUE(DissolveSys::isNumber("+5.712945e+4", isFP));
    EXPECT_TRUE(isFP);
    EXPECT_TRUE(DissolveSys::isNumber("100", isFP));
    EXPECT_FALSE(isFP);
    EXPECT_FALSE(DissolveSys::isNumber("abc", isFP));
    EXPECT_FALSE(DissolveSys::isNumber(".", isFP));
    EXPECT_FALSE(isFP);
    EXPECT_FALSE(DissolveSys::isNumber("", isFP));
    EXPECT_FALSE(isFP);
    EXPECT_FALSE(DissolveSys::isNumber("e-1", isFP));
    EXPECT_FALSE(isFP);
}

TEST(SysFunc, StringReplacement)
{
    // String replacement
    EXPECT_TRUE(DissolveSys::replace("Swap the aaa's", "a", "bee") == "Swbeep the beebeebee's");
    EXPECT_TRUE(DissolveSys::replace("Swap the AaA's", "A", "bee") == "Swap the beeabee's");
}

TEST(SysFunc, StringSplitSimple)
{
    // String splitting / joining
    std::vector<std::string_view> v;
    const std::vector<std::string_view> expected = {"arma", "dillo", "is", "in", "pieces"};
    // -- Empty string
    v = DissolveSys::splitString("", " ");
    EXPECT_EQ(v.size(), 0);
    // -- Simple splitting
    v = DissolveSys::splitString("arma dillo is in pieces", " ");
    EXPECT_EQ(v.size(), 5);
    EXPECT_TRUE(expected == v);
    v = DissolveSys::splitString("a b c d e", "c");
    EXPECT_EQ(v.size(), 2);
    EXPECT_TRUE(v[0] == "a b ");
    EXPECT_TRUE(v[1] == " d e");
    // -- Consecutive delimiters
    v = DissolveSys::splitString("arma   dillo is in    pieces", " ");
    EXPECT_EQ(v.size(), 5);
    EXPECT_TRUE(expected == v);
    // -- Add trailing delimiters
    v = DissolveSys::splitString("arma   dillo is in    pieces         ", " ");
    EXPECT_EQ(v.size(), 5);
    EXPECT_TRUE(expected == v);
    // -- Add leading delimiters
    v = DissolveSys::splitString("   arma   dillo is in    pieces         ", " ");
    EXPECT_EQ(v.size(), 5);
    EXPECT_TRUE(expected == v);
}

TEST(SysFunc, StringSplitMultiple)
{
    const std::vector<std::string_view> abcd = {"A", "B", "C", "D"};

    // -- Two character delimiter
    auto split = DissolveSys::splitString("A//B//C//D", "//");
    EXPECT_EQ(split.size(), 4);
    EXPECT_TRUE(split == abcd);

    // -- Trailing two character delimiter
    split = DissolveSys::splitString("A//B//C//D//", "//");
    EXPECT_EQ(split.size(), 4);
    EXPECT_TRUE(split == abcd);

    // -- Leading two character delimiter
    split = DissolveSys::splitString("//A//B//C//D", "//");
    EXPECT_EQ(split.size(), 4);
    EXPECT_TRUE(split == abcd);

    // -- Leading and trailing two character delimiter
    split = DissolveSys::splitString("//A//B//C//D//", "//");
    EXPECT_EQ(split.size(), 4);
    EXPECT_TRUE(split == abcd);

    // -- Larger delimiter
    split = DissolveSys::splitString("AanklesBanklesCanklesDankles", "ankles");
    EXPECT_EQ(split.size(), 4);
    EXPECT_TRUE(split == abcd);
}

TEST(SysFunc, StringStripping)
{
    // String strip
    EXPECT_EQ(DissolveSys::strip("No more spaces "), "Nomorespaces");
    EXPECT_EQ(DissolveSys::strip("No more spaces ", 'o'), "N mre spaces ");
}

TEST(SysFunc, UniqueNames)
{
    // Unique Naming
    std::vector<NameObject> names;
    EXPECT_TRUE(DissolveSys::uniqueName("IAmUnique", names, [](const auto &obj) { return obj.name; }) == "IAmUnique");
    names.emplace_back("IAmUnique");
    EXPECT_FALSE(DissolveSys::uniqueName("IAmUnique", names, [](const auto &obj) { return obj.name; }) == "IAmUnique");
    EXPECT_TRUE(DissolveSys::uniqueName("IAmUnique", names, [](const auto &obj) { return obj.name; }) == "IAmUnique01");
    auto &exc = names.emplace_back("IAmUnique01");
    EXPECT_FALSE(DissolveSys::uniqueName("IAmUnique", names, [](const auto &obj) { return obj.name; }) == "IAmUnique01");
    EXPECT_TRUE(DissolveSys::uniqueName("IAmUnique", names, [](const auto &obj) { return obj.name; }) == "IAmUnique02");
    EXPECT_TRUE(DissolveSys::uniqueName("IAmUnique", names, [&](const auto &obj) { return &exc == &obj ? "" : obj.name; }) ==
                "IAmUnique01");
}

} // namespace UnitTest

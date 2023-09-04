// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "templates/flags.h"
#include <gtest/gtest.h>

namespace RedDwarf
{
enum Crew
{
    Cat,
    Holly,
    Kryten,
    Lister,
    Rimmer
};
};

TEST(FlagsTest, Basic)
{
    // Add single signal
    Flags<RedDwarf::Crew> blueMidget;
    blueMidget += RedDwarf::Crew::Lister;
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Lister));

    // Reset by int assignment
    blueMidget = 0;
    EXPECT_FALSE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Lister));

    // Construct from single enum
    Flags<RedDwarf::Crew> starBug1(RedDwarf::Crew::Rimmer);
    EXPECT_FALSE(starBug1.isSet(RedDwarf::Crew::Cat));
    EXPECT_FALSE(starBug1.isSet(RedDwarf::Crew::Holly));
    EXPECT_FALSE(starBug1.isSet(RedDwarf::Crew::Kryten));
    EXPECT_FALSE(starBug1.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(starBug1.isSet(RedDwarf::Crew::Rimmer));

    // Construct from integer
    Flags<RedDwarf::Crew> starBug2(11);
    EXPECT_TRUE(starBug2.isSet(RedDwarf::Crew::Cat));
    EXPECT_TRUE(starBug2.isSet(RedDwarf::Crew::Holly));
    EXPECT_FALSE(starBug2.isSet(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(starBug2.isSet(RedDwarf::Crew::Lister));
    EXPECT_FALSE(starBug2.isSet(RedDwarf::Crew::Rimmer));
}

TEST(FlagsTest, Manipulation)
{
    Flags<RedDwarf::Crew> blueMidget;

    // Add multiple blueMidget
    blueMidget += RedDwarf::Crew::Cat;
    blueMidget += RedDwarf::Crew::Lister;
    blueMidget += RedDwarf::Crew::Kryten;
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Cat));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Cat));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Lister));

    // Remove one, test again
    blueMidget -= RedDwarf::Crew::Kryten;
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Cat));
    EXPECT_FALSE(blueMidget.isSet(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Cat));
    EXPECT_FALSE(blueMidget.isSetOrNone(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Lister));

    // Convert to int, clear, set, and test again
    int i = blueMidget;
    blueMidget = 0;
    blueMidget = i;
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Cat));
    EXPECT_FALSE(blueMidget.isSet(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Cat));
    EXPECT_FALSE(blueMidget.isSetOrNone(RedDwarf::Crew::Kryten));
    EXPECT_TRUE(blueMidget.isSetOrNone(RedDwarf::Crew::Lister));
}

TEST(FlagsTest, Combination)
{
    Flags<RedDwarf::Crew> blueMidget(RedDwarf::Crew::Lister, RedDwarf::Crew::Cat, RedDwarf::Crew::Rimmer);
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Cat));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Rimmer));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_FALSE(blueMidget.isSet(RedDwarf::Crew::Holly));
    EXPECT_FALSE(blueMidget.isSet(RedDwarf::Crew::Kryten));

    Flags<RedDwarf::Crew> starBug(RedDwarf::Crew::Holly, RedDwarf::Crew::Kryten);
    EXPECT_FALSE(starBug.isSet(RedDwarf::Crew::Cat));
    EXPECT_FALSE(starBug.isSet(RedDwarf::Crew::Rimmer));
    EXPECT_FALSE(starBug.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(starBug.isSet(RedDwarf::Crew::Holly));
    EXPECT_TRUE(starBug.isSet(RedDwarf::Crew::Kryten));

    blueMidget |= starBug;
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Cat));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Rimmer));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Lister));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Holly));
    EXPECT_TRUE(blueMidget.isSet(RedDwarf::Crew::Kryten));
}

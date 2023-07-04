// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/enumOptions.h"
#include "base/enumOption.h"
#include <gtest/gtest.h>

TEST(EnumOptionsTest, EnumOptions)
{
    // Create some enumerated options to test
    enum Condition
    {
        Red,
        Mauve,
        Magenta,
        Taupe,
        Marigold,
        Heliotrope,
        Tangerine
    };
    EnumOptions<Condition> options("TestOptions", {{Condition::Red, "Red"},
                                                   {Condition::Mauve, "Mauve", OptionArguments::OneOrMore},
                                                   {Condition::Magenta, "Magenta", OptionArguments::OptionalSecond},
                                                   {Condition::Taupe, "Taupe", 3, 6},
                                                   {Condition::Marigold, "Marigold", 4},
                                                   {Condition::Heliotrope, "Heliotrope", 3, 3},
                                                   {Condition::Tangerine, "Tangerine", 2, OptionArguments::AnyNumber}});

    // Test options
    // Red - no arguments accepted
    ASSERT_TRUE(options.validNArgs(Condition::Red, 0));
    ASSERT_FALSE(options.validNArgs(Condition::Red, 1));
    // Mauve - One or more arguments
    ASSERT_TRUE(options.validNArgs(Condition::Mauve, 1));
    ASSERT_TRUE(options.validNArgs(Condition::Mauve, 99));
    ASSERT_FALSE(options.validNArgs(Condition::Mauve, 0));
    // Magenta - One or two arguments
    ASSERT_TRUE(options.validNArgs(Condition::Magenta, 1));
    ASSERT_TRUE(options.validNArgs(Condition::Magenta, 2));
    ASSERT_FALSE(options.validNArgs(Condition::Magenta, 0));
    ASSERT_FALSE(options.validNArgs(Condition::Magenta, 10));
    // Taupe - Three to six arguments
    ASSERT_TRUE(options.validNArgs(Condition::Taupe, 3));
    ASSERT_TRUE(options.validNArgs(Condition::Taupe, 4));
    ASSERT_TRUE(options.validNArgs(Condition::Taupe, 5));
    ASSERT_TRUE(options.validNArgs(Condition::Taupe, 6));
    ASSERT_FALSE(options.validNArgs(Condition::Taupe, 2));
    ASSERT_FALSE(options.validNArgs(Condition::Taupe, 7));
    // Marigold - Exactly four arguments
    ASSERT_TRUE(options.validNArgs(Condition::Marigold, 4));
    ASSERT_FALSE(options.validNArgs(Condition::Marigold, 0));
    ASSERT_FALSE(options.validNArgs(Condition::Marigold, 2));
    ASSERT_FALSE(options.validNArgs(Condition::Marigold, 5));
    // Heliotrope - Exactly three arguments (min and max specified)
    ASSERT_TRUE(options.validNArgs(Condition::Heliotrope, 3));
    ASSERT_FALSE(options.validNArgs(Condition::Heliotrope, 2));
    ASSERT_FALSE(options.validNArgs(Condition::Heliotrope, 4));
    // Tangerine - At least two arguments
    ASSERT_TRUE(options.validNArgs(Condition::Tangerine, 2));
    ASSERT_FALSE(options.validNArgs(Condition::Tangerine, 0));
    ASSERT_FALSE(options.validNArgs(Condition::Tangerine, 1));
    ASSERT_TRUE(options.validNArgs(Condition::Tangerine, 10));
    ASSERT_TRUE(options.validNArgs(Condition::Tangerine, 99));
}

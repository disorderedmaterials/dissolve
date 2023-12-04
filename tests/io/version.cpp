// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/version.h"
#include "base/serialiser.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(VersionTest, VersionClass)
{
    using Version::DissolveVersion;
    DissolveVersion first{"1.10.3"}, second{"2.1.12"}, minor{"2.2.1"}, patch{"2.2.2"}, third{"3.0.0"};

    // Main sequence
    EXPECT_LT(first, second);
    EXPECT_LT(second, minor);
    EXPECT_LT(minor, patch);
    EXPECT_LT(patch, third);

    // Full combinatorial
    EXPECT_LT(first, minor);
    EXPECT_LT(first, patch);
    EXPECT_LT(first, third);
    EXPECT_LT(second, minor);
    EXPECT_LT(second, patch);
    EXPECT_LT(second, third);
    EXPECT_LT(minor, third);
}

TEST(VersionTest, VersionInfo)
{
    CoreData coreData;
    Dissolve dissolve(coreData);
    dissolve.loadInput("dissolve/input/rdfMethod.txt");
    auto serialised = dissolve.serialise();

    Version::DissolveVersion fileVersion(serialised["version"].as_string().str), actual(Version::semantic());

    EXPECT_EQ(fileVersion, actual);
}

TEST(VersionTest, VersionUpgrade)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    SerialisedValue old;
    old["version"] = "0.0.0";
    old["pairPotentials"] = {{"coulombTruncation", "Shift"}};

    dissolve.deserialise(old);
}

} // namespace UnitTest

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/version.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(VersionTest, VersionInfo)
{
    CoreData coreData;
    Dissolve dissolve(coreData);
    dissolve.loadInput("dissolve/input/rdfMethod.txt");
    auto serialised = dissolve.serialise();

    EXPECT_EQ(serialised["version"].as_string().str, Version::semantic());
}

} // namespace UnitTest

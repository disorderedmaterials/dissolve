// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/species.h"
#include "nodes/constants.h"
#include "nodes/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class SpeciesNodeTest : public ::testing::Test
{
    public:
    SpeciesNodeTest() : dissolve_(coreData_), root_(dissolve_) {}

    protected:
    // We need a CoreData and Dissolve definition to properly
    // instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
};

TEST_F(SpeciesNodeTest, Creation)
{
    auto s = dynamic_cast<SpeciesNode *>(root_.createNode("Species", "s"));
    ASSERT_EQ(s->name(), "s");
    auto option = s->findOption("Definition");
    ASSERT_NE(option, nullptr);
    option->set<std::string>("Hello, World!");
    ASSERT_EQ(s->run(), NodeConstants::ProcessResult::Failed);
}

}; // namespace UnitTest

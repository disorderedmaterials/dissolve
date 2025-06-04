// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/test.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ParametersTest, OptionalPointerOutput)
{
    CoreData coreData_;
    Dissolve dissolve_(coreData_);
    DissolveGraph root_(dissolve_);

    // Create a couple of TestNodes
    auto *a = dynamic_cast<TestNode *>(root_.addNode(std::make_unique<TestNode>(&root_), "TestA"));
    ASSERT_TRUE(a);
    auto *b = dynamic_cast<TestNode *>(root_.addNode(std::make_unique<TestNode>(&root_), "TestB"));
    ASSERT_TRUE(b);

    // Create an edge between nodes
    ASSERT_TRUE(root_.addEdge({"TestA", "OptionalConfiguration", "TestB", "ConfigurationInput"}));

    // Inputs to TestB should be valid (there is no optional data yet, but the Edge definitions are correct)
    EXPECT_TRUE(b->inputsAreValid());
    EXPECT_FALSE(b->isUpToDate());
    EXPECT_EQ(b->versionIndex(), NodeConstants::InvalidVersion);

    // Running TestB should fail since there is no optional data to retrieve via the Edge
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Failed);
    // We have a choice of where this detection could be done.  Either Node::run() could check outputs for validity, or we
    // could have a default flag on a PointerParameter to disallow nullptr, or we could somehow modify Parameter<T>::assign()
    // to disallow pointer parameters if the underlying type is a pointer??
}

} // namespace UnitTest

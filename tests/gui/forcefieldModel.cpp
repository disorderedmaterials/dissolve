// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "gui/models/simpleForcefieldModel.h"
#include "nodes/forcefield.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ForcefieldModel, Basic)
{
    // Stage 1: Simulate GUI interactions to create species
    Graph graph(nullptr);
    GraphModel graphModel;
    graphModel.setGraph(&graph);

    ASSERT_EQ(graph.nodes().size(), 2);

    SimpleForcefieldModel model;
    model.assignModel(&graphModel);
    model.setName("Kulmala2010");
    model.create(0, 0);

    ASSERT_EQ(graph.nodes().size(), 3);
    auto node = dynamic_cast<ForcefieldNode *>(graph.findNode("Kulmala2010"));
    ASSERT_TRUE(node);
    EXPECT_EQ(node->forcefield().get(), ForcefieldLibrary::forcefield("Kulmala2010").get());
}
} // namespace UnitTest

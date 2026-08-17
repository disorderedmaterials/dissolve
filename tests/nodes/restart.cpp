// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "src/nodes/gr.h"
#include "tests/testGraph.h"
#include <filesystem>

namespace UnitTest
{
TEST(RestartTest, Configuration)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"Ar", 5000}}, 0.0213)) << "Created Configuration";
    auto sqNode = testGraph.appendGRSQ(true, true);
    auto grNode = testGraph.findNode("GR");
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::TestMethod));
    ASSERT_TRUE(grNode) << "Valid GR Node";
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);

    auto configurationNode = testGraph.findNode("Box");
    ASSERT_TRUE(configurationNode) << "Valid configuration node";

    auto path = std::filesystem::temp_directory_path() / "dissolve" / "RestartTest";
    std::filesystem::create_directories(path);
    configurationNode->saveRestart(path);
};

} // namespace UnitTest

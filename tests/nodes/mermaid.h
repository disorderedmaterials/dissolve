// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/graph.h"
#include <gtest/gtest.h>
#include <optional>
#include <string>

namespace UnitTest
{

// Save the graph in Mermaid format to a file named after the unit test
void exportMermaidGraph(Graph &graph, std::optional<std::string> context = {})
{
    auto suite = ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
    auto name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    std::string filename = std::format("{}-{}.mermaid", suite, name);
    if (context)
        filename = std::format("{}-{}-{}.mermaid", suite, name, filename);

    std::ofstream myfile;
    myfile.open(filename);
    myfile << graph;
    myfile.close();
}
} // namespace UnitTest

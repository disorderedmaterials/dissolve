// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/gr/gr.h"
#include "benchmark/benchmark.h"
#include "common/problems.h"
#include "nodes/number.h"
#include <optional>

namespace Benchmarks
{
template <SystemType systemType, GRNode::PartialsMethod method> static void BM_GRNode(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;

    // Create and set up a GR node
    auto *grNode = problemDef.createNode("GR");
    problemDef.addEdge({"SetCoordinates", "Configuration", "GR", "Configuration"});
    grNode->setOption("Method", method);
    grNode->setOption("Averaging", std::optional<Number>(0));
    grNode->setOption("IntraBroadening", Function1DWrapper());

    for (auto _ : state)
    {
        grNode->run();
        grNode->setUpdateRequired();
    }
}

BENCHMARK_TEMPLATE(BM_GRNode, SystemType::ArgonCubic, GRNode::PartialsMethod::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_GRNode, SystemType::ArgonCubic, GRNode::PartialsMethod::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_GRNode, SystemType::Water1000, GRNode::PartialsMethod::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(BM_GRNode, SystemType::Water1000, GRNode::PartialsMethod::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(BM_GRNode, SystemType::Hexane200, GRNode::PartialsMethod::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(BM_GRNode, SystemType::Hexane200, GRNode::PartialsMethod::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMicrosecond);

} // namespace Benchmarks

BENCHMARK_MAIN();

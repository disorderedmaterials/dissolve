// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/gr/gr.h"
#include "benchmark/benchmark.h"
#include "common/problems.h"

namespace Benchmarks
{
template <SpeciesType speciesType, SpeciesPopulation population, GRNode::PartialsMethod method>
static void BM_CalculateGR(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;

    // Setup the GR node
    GRNode grNode(nullptr);
    grNode.setOption<Configuration *>("Configurations", problemDef.configuration());

    double rdfRange = problemDef.configuration()->box()->inscribedSphereRadius();
    bool upToDate = false;
    // for (auto _ : state)
    // {
    // grNode.calculateGR(problemDef.dissolve().processingModuleData(), problemDef.configuration(), method, rdfRange, 0.05,
    // upToDate);
    // problemDef.dissolve().processingModuleData().clearAll();
    // }
}
//
// BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Small, GRNode::PartialsMethod::SimpleMethod)
//     ->Iterations(5)
//     ->Unit(benchmark::kMillisecond);
// BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Small, GRNode::PartialsMethod::CellsMethod)
//     ->Iterations(5)
//     ->Unit(benchmark::kMillisecond);
// BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Medium, GRNode::PartialsMethod::SimpleMethod)
//     ->Iterations(5)
//     ->Unit(benchmark::kMillisecond);
// BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Medium, GRNode::PartialsMethod::CellsMethod)
//     ->Iterations(5)
//     ->Unit(benchmark::kMillisecond);
// BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Large, GRNode::PartialsMethod::SimpleMethod)
//     ->Iterations(5)
//     ->Unit(benchmark::kMillisecond);
// BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Large, GRNode::PartialsMethod::CellsMethod)
//     ->Iterations(5)
//     ->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();

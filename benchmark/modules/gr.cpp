// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/gr/gr.h"
#include "benchmark/benchmark.h"
#include "common/problems.h"

namespace Benchmarks
{
template <SpeciesType speciesType, SpeciesPopulation population, GRModule::PartialsMethod method>
static void BM_CalculateGR(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;

    // Setup the GR module
    auto grModule = std::make_unique<GRModule>();
    grModule->keywords().set("Configurations", std::vector<Configuration *>{problemDef.configuration()});

    double rdfRange = problemDef.configuration()->box()->inscribedSphereRadius();
    bool upToDate = false;
    for (auto _ : state)
    {
        grModule->calculateGR(problemDef.dissolve().processingModuleData(), problemDef.dissolve().worldPool(),
                              problemDef.configuration(), method, rdfRange, 0.05, upToDate);
        problemDef.dissolve().processingModuleData().clearAll();
    }
}

BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Small, GRModule::PartialsMethod::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Small, GRModule::PartialsMethod::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Medium, GRModule::PartialsMethod::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Medium, GRModule::PartialsMethod::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Large, GRModule::PartialsMethod::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, SpeciesType::Atomic, SpeciesPopulation::Large, GRModule::PartialsMethod::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "common/problems.h"

template <ProblemType problem, Population population, GRModule::PartialsMethod method>
static void BM_CalculateGR(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    for (auto _ : state)
        problemDef.template iterateGR<method>();
}

BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::atomic, Population::small, Method::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::atomic, Population::small, Method::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::atomic, Population::medium, Method::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::atomic, Population::medium, Method::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::atomic, Population::large, Method::SimpleMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::atomic, Population::large, Method::CellsMethod)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

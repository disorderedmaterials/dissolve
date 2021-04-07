
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "benchmark/benchmark.h"
#include "test_problem.h"

template <ProblemSize problem, RDFModule::PartialsMethod method> static void BM_CalculateGR(benchmark::State &state)
{
    Problem<problem> problemDef;
    for (auto _ : state)
    {
        problemDef.template iterateCells<method>();
    }
}

BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemSize::Small, Method::SimpleMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemSize::Small, Method::CellsMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemSize::Medium, Method::SimpleMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemSize::Medium, Method::CellsMethod)->Iterations(5)->Unit(benchmark::kMillisecond);

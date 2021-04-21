// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "benchmark/benchmark.h"
#include "common/problems.h"

template <ProblemType problem, RDFModule::PartialsMethod method> static void BM_CalculateGR(benchmark::State &state)
{
    Problem<problem> problemDef;
    for (auto _ : state)
        problemDef.template iterateGR<method>();
}

BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::Argon1k, Method::SimpleMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::Argon1k, Method::CellsMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::Argon5k, Method::SimpleMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::Argon5k, Method::CellsMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::Argon10k, Method::SimpleMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateGR, ProblemType::Argon10k, Method::CellsMethod)->Iterations(5)->Unit(benchmark::kMillisecond);
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "classes/box.h"
#include "classes/region.h"
#include "common/problems.h"
#include <cmath>

template <ProblemType problem, Population population> static void BM_Region_Generate(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    for (auto _ : state)
    {
        Region region;
        region.generate(cfg, 1.0, [&](const Configuration *cfg, Vec3<double> r) { return DissolveMath::random() > 0.5; });
    }
}

BENCHMARK_TEMPLATE(BM_Region_Generate, ProblemType::mediumMolecule, Population::small);

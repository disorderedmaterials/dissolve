// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "classes/box.h"
#include "classes/region.h"
#include "common/problems.h"
#include <cmath>

class RandomVoxelKernel : public VoxelKernel
{
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override { return DissolveMath::random() > 0.5; }
};

template <ProblemType problem, Population population> static void BM_Region_Generate(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    for (auto _ : state)
    {
        Region region;
        region.generate(cfg, 1.0, []() { return std::make_shared<RandomVoxelKernel>(); });
    }
}

BENCHMARK_TEMPLATE(BM_Region_Generate, ProblemType::mediumMolecule, Population::small);

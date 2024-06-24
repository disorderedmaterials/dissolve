// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/region.h"
#include "classes/box.h"
#include "common/problems.h"
#include <benchmark/benchmark.h>
#include <cmath>

class RandomVoxelKernel : public VoxelKernel
{
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override { return DissolveMath::random() > 0.5; }
};

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_Region_Generate(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    for (auto _ : state)
    {
        Region region;
        region.generate(cfg, 1.0, false, []() { return std::make_shared<RandomVoxelKernel>(); });
    }
}

BENCHMARK_TEMPLATE(BM_Region_Generate, DissolveBenchmarks::SpeciesType::MediumMolecule, DissolveBenchmarks::Population::Small);

BENCHMARK_MAIN();

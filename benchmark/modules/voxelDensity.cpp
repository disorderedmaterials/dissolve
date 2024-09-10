// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "common/problems.h"
#include "module/context.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
template <SpeciesType speciesType, SpeciesPopulation population, VoxelDensityModule::TargetPropertyType property>
static void BM_Module_VoxelDensity(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    VoxelDensityModule module;
    module.keywords().set("Configuration", problemDef.configuration());
    module.keywords().set("BinRange", Vec3<double>(0.0, 10.0, 0.05));
    module.keywords().set("VoxelSideLength", 1.0);
    module.keywords().setEnumeration("TargetProperty", property);
    ModuleContext context(problemDef.dissolve().worldPool(), problemDef.dissolve());
    for (auto _ : state)
    {
        problemDef.configuration()->incrementContentsVersion();
        module.executeProcessing(context);
    }
}

BENCHMARK_TEMPLATE(BM_Module_VoxelDensity, SpeciesType::SmallMolecule, SpeciesPopulation::Small,
                   VoxelDensityModule::TargetPropertyType::Mass)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Module_VoxelDensity, SpeciesType::Atomic, SpeciesPopulation::Large,
                   VoxelDensityModule::TargetPropertyType::Mass)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Module_VoxelDensity, SpeciesType::SmallMolecule, SpeciesPopulation::Small,
                   VoxelDensityModule::TargetPropertyType::AtomicNumber)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Module_VoxelDensity, SpeciesType::Atomic, SpeciesPopulation::Large,
                   VoxelDensityModule::TargetPropertyType::AtomicNumber)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Module_VoxelDensity, SpeciesType::SmallMolecule, SpeciesPopulation::Small,
                   VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Module_VoxelDensity, SpeciesType::Atomic, SpeciesPopulation::Large,
                   VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity)
    ->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/energy.h"
#include "common/problems.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
// Single atom energy in different box shapes
template <SystemType systemType> static void BM_CalculateEnergy_AtomicWorldEnergy(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;
    auto energyKernel = problemDef.createEnergyKernel(problemDef.configuration());
    auto &i = problemDef.configuration()->atom(0);
    for (auto _ : state)
        energyKernel->totalEnergy(i);
}
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, SystemType::ArgonCubic);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, SystemType::ArgonMonoclinic);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, SystemType::ArgonTriclinic);

// Geometric energies
template <SystemType systemType> static void BM_CalculateEnergy_GeometryEnergy(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;
    auto energyKernel = problemDef.createEnergyKernel(problemDef.configuration());
    for (auto _ : state)
        energyKernel->totalEnergy({Kernel::CalculationFlags::ExcludeInterMolecularPairPotential,
                                   Kernel::CalculationFlags::ExcludeIntraMolecularPairPotential});
}
BENCHMARK_TEMPLATE(BM_CalculateEnergy_GeometryEnergy, SystemType::Water1000);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_GeometryEnergy, SystemType::Hexane200);

// Pair potential energies
template <SystemType systemType> static void BM_CalculateEnergy_PairPotentialEnergy(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;
    auto energyKernel = problemDef.createEnergyKernel(problemDef.configuration());
    for (auto _ : state)
        energyKernel->totalEnergy({Kernel::CalculationFlags::ExcludeGeometric});
}
BENCHMARK_TEMPLATE(BM_CalculateEnergy_PairPotentialEnergy, SystemType::ArgonCubic);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_PairPotentialEnergy, SystemType::ArgonMonoclinic);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_PairPotentialEnergy, SystemType::ArgonTriclinic);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_PairPotentialEnergy, SystemType::Water1000);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_PairPotentialEnergy, SystemType::Hexane200);
} // namespace Benchmarks

BENCHMARK_MAIN();

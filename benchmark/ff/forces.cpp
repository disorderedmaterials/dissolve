// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "common/problems.h"
#include "kernels/force.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
// Single atom energy in different box shapes
template <SystemType systemType> static void BM_CalculateForces_AtomicWorldForces(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;
    auto forceKernel = problemDef.createForceKernel(problemDef.configuration());
    auto &i = problemDef.configuration()->atom(0);
    std::vector<Vector3> ppForces, geometryForces;
    for (auto _ : state)
        forceKernel->totalForces(ppForces, geometryForces);
}
BENCHMARK_TEMPLATE(BM_CalculateForces_AtomicWorldForces, SystemType::ArgonCubic);
BENCHMARK_TEMPLATE(BM_CalculateForces_AtomicWorldForces, SystemType::ArgonMonoclinic);
BENCHMARK_TEMPLATE(BM_CalculateForces_AtomicWorldForces, SystemType::ArgonTriclinic);

// Geometric energies
template <SystemType systemType> static void BM_CalculateForces_GeometryForces(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;
    auto forceKernel = problemDef.createForceKernel(problemDef.configuration());
    std::vector<Vector3> ppForces, geometryForces;
    for (auto _ : state)
        forceKernel->totalForces(ppForces, geometryForces,
                                 {Kernel::CalculationFlags::ExcludeInterMolecularPairPotential,
                                  Kernel::CalculationFlags::ExcludeIntraMolecularPairPotential});
}
BENCHMARK_TEMPLATE(BM_CalculateForces_GeometryForces, SystemType::Water1000);
BENCHMARK_TEMPLATE(BM_CalculateForces_GeometryForces, SystemType::Hexane200);

// Pair potential energies
template <SystemType systemType> static void BM_CalculateForces_PairPotentialForces(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;
    auto forceKernel = problemDef.createForceKernel(problemDef.configuration());
    std::vector<Vector3> ppForces, geometryForces;
    for (auto _ : state)
        forceKernel->totalForces(ppForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});
}
BENCHMARK_TEMPLATE(BM_CalculateForces_PairPotentialForces, SystemType::ArgonCubic);
BENCHMARK_TEMPLATE(BM_CalculateForces_PairPotentialForces, SystemType::ArgonMonoclinic);
BENCHMARK_TEMPLATE(BM_CalculateForces_PairPotentialForces, SystemType::ArgonTriclinic);
BENCHMARK_TEMPLATE(BM_CalculateForces_PairPotentialForces, SystemType::Water1000);
BENCHMARK_TEMPLATE(BM_CalculateForces_PairPotentialForces, SystemType::Hexane200);
} // namespace Benchmarks

BENCHMARK_MAIN();

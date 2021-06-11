// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "benchmark/benchmark.h"
#include "classes/cell.h"
#include "classes/forcekernel.h"
#include "classes/species.h"
#include "common/problems.h"
#include "modules/forces/forces.h"

template <ProblemType problem, Population population> ForceKernel createForceKernel(Problem<problem, population> &problemDef)
{

    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    auto *cfg = problemDef.cfg_;
    ForceKernel kernel(procPool, cfg->box(), cfg->cells(), potentialMap);
    return kernel;
}

template <ProblemType problem, Population population> static void BM_CalculateForces_InterAtomic(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &cellArray = problemDef.cfg_->cells();
    auto *cellI = cellArray.cell(1);
    for (auto _ : state)
    {
        forceKernel.forces(cellI, true, ProcessPool::PoolStrategy, forces);
    }
}

template <ProblemType problem, Population population> static void BM_CalculateForces_SpeciesBond(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    const auto &bond = mol->species()->bonds().back();

    for (auto _ : state)
    {
        forceKernel.forces(bond, forces);
    }
}

template <ProblemType problem, Population population> static void BM_CalculateForces_SpeciesAngle(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    const auto &angle = mol->species()->angles().back();
    for (auto _ : state)
    {
        forceKernel.forces(angle, forces);
    }
}
template <ProblemType problem, Population population> static void BM_CalculateForces_SpeciesTorsion(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    const auto &torsion = mol->species()->torsions().back();
    for (auto _ : state)
    {
        forceKernel.forces(torsion, forces);
    }
}

template <ProblemType problem, Population population>
static void BM_CalculateForces_TotalIntraMolecular(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
        ForcesModule::intraMolecularForces(procPool, cfg, potentialMap, forces);
}

template <ProblemType problem, Population population> static void BM_CalculateForces_TotalInterAtomic(benchmark::State &state)
{

    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
    {
        std::vector<Vec3<double>> forces(cfg->nAtoms());
        ForcesModule::interAtomicForces(procPool, cfg, potentialMap, forces);
    }
}

template <ProblemType problem, Population population> static void BM_CalculateForces_TotalForces(benchmark::State &state)
{

    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
    {
        std::vector<Vec3<double>> forces(cfg->nAtoms());
        ForcesModule::totalForces(procPool, cfg, potentialMap, forces);
    }
}
// small molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_InterAtomic, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, ProblemType::smallMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, ProblemType::smallMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// medium molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_InterAtomic, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesTorsion, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, ProblemType::mediumMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, ProblemType::mediumMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

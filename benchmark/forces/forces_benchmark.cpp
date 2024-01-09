// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "classes/cell.h"
#include "common/problems.h"
#include "kernels/producer.h"
#include "modules/forces/forces.h"

template <ProblemType problem, Population population>
std::unique_ptr<ForceKernel> createForceKernel(Problem<problem, population> &problemDef)
{

    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    auto *cfg = problemDef.cfg_;
    return KernelProducer::forceKernel(cfg, procPool, potentialMap);
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
        forceKernel->bondForces(bond, bond.i()->r(), bond.j()->r(), forces);
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
        forceKernel->angleForces(angle, angle.i()->r(), angle.j()->r(), angle.k()->r(), forces);
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
        forceKernel->torsionForces(torsion, torsion.i()->r(), torsion.j()->r(), torsion.k()->r(), torsion.l()->r(), forces);
}

template <ProblemType problem, Population population>
static void BM_CalculateForces_TotalIntraMolecular(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    for (auto _ : state)
        forceKernel->totalForces(
            forces, forces, ProcessPool::PoolStrategy,
            {ForceKernel::ExcludeInterMolecularPairPotential, ForceKernel::ExcludeIntraMolecularPairPotential});
}

template <ProblemType problem, Population population> static void BM_CalculateForces_TotalSpecies(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &sp = problemDef.coreData_.species().front();
    std::vector<Vec3<double>> forces(sp->nAtoms());
    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
        ForcesModule::totalForces(procPool, sp.get(), potentialMap, ForcesModule::ForceCalculationType::Full, forces, forces);
}

template <ProblemType problem, Population population> static void BM_CalculateForces_TotalInterAtomic(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    for (auto _ : state)
        forceKernel->totalForces(forces, forces, ProcessPool::PoolStrategy, ForceKernel::ExcludeGeometry);
}

template <ProblemType problem, Population population> static void BM_CalculateForces_TotalForces(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto *cfg = problemDef.cfg_;
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
        ForcesModule::totalForces(procPool, cfg, potentialMap, ForcesModule::ForceCalculationType::Full, forces, forces);
}
// small molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, ProblemType::smallMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, ProblemType::smallMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// medium molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesTorsion, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, ProblemType::mediumMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, ProblemType::mediumMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// framework molecule benchmarks
// BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, ProblemType::frameworkMolecule, Population::single)
//    ->Unit(benchmark::kMillisecond);

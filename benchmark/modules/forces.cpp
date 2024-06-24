// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/forces/forces.h"
#include "classes/cell.h"
#include "common/problems.h"
#include "kernels/producer.h"
#include <benchmark/benchmark.h>

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
std::unique_ptr<ForceKernel> createForceKernel(DissolveBenchmarks::Problem<speciesType, population> &problemDef)
{

    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    auto *cfg = problemDef.configuration();
    return KernelProducer::forceKernel(cfg, procPool, potentialMap);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_SpeciesBond(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &bond = mol->species()->bonds().back();

    for (auto _ : state)
        forceKernel->bondForces(bond, bond.i()->r(), bond.j()->r(), forces);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_SpeciesAngle(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &angle = mol->species()->angles().back();
    for (auto _ : state)
        forceKernel->angleForces(angle, angle.i()->r(), angle.j()->r(), angle.k()->r(), forces);
}
template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_SpeciesTorsion(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &torsion = mol->species()->torsions().back();
    for (auto _ : state)
        forceKernel->torsionForces(torsion, torsion.i()->r(), torsion.j()->r(), torsion.k()->r(), torsion.l()->r(), forces);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_TotalIntraMolecular(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    for (auto _ : state)
        forceKernel->totalForces(
            forces, forces, ProcessPool::PoolStrategy,
            {ForceKernel::ExcludeInterMolecularPairPotential, ForceKernel::ExcludeIntraMolecularPairPotential});
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_TotalSpecies(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto &sp = problemDef.coreData().species().front();
    std::vector<Vec3<double>> forces(sp->nAtoms());
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        ForcesModule::totalForces(procPool, sp.get(), potentialMap, ForcesModule::ForceCalculationType::Full, forces, forces);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_TotalInterAtomic(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    for (auto _ : state)
        forceKernel->totalForces(forces, forces, ProcessPool::PoolStrategy, ForceKernel::ExcludeGeometry);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateForces_TotalForces(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vec3<double>> forces(cfg->nAtoms());
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        ForcesModule::totalForces(procPool, cfg, potentialMap, ForcesModule::ForceCalculationType::Full, forces, forces);
}
// small molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// medium molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesTorsion, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// framework molecule benchmarks
// BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, ProblemType::frameworkMolecule, Population::single)
//    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

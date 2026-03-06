// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "modules/forces/forces.h"
#include "classes/cell.h"
#include "common/problems.h"
#include "kernels/producer.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
template <SpeciesType speciesType, SpeciesPopulation population>
std::unique_ptr<ForceKernel> createForceKernel(Problem<speciesType, population> &problemDef)
{
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    auto *cfg = problemDef.configuration();
    return KernelProducer::forceKernel(cfg, potentialMap);
}

template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_SpeciesBond(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vector3> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &bond = mol->species()->bonds().back();

    for (auto _ : state)
        forceKernel->bondForces(bond, bond.i()->r(), bond.j()->r(), forces);
}

template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_SpeciesAngle(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vector3> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &angle = mol->species()->angles().back();
    for (auto _ : state)
        forceKernel->angleForces(angle, angle.i()->r(), angle.j()->r(), angle.k()->r(), forces);
}
template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_SpeciesTorsion(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vector3> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &torsion = mol->species()->torsions().back();
    for (auto _ : state)
        forceKernel->torsionForces(torsion, torsion.i()->r(), torsion.j()->r(), torsion.k()->r(), torsion.l()->r(), forces);
}

template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_TotalIntraMolecular(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vector3> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    for (auto _ : state)
        forceKernel->totalForces(forces, forces,
                                 {Kernel::ExcludeInterMolecularPairPotential, Kernel::ExcludeIntraMolecularPairPotential});
}

template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_TotalSpecies(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto &sp = problemDef.coreData().species().front();
    std::vector<Vector3> ppForces, geomForces;
    auto kernel = KernelProducer::speciesKernel(sp.get(), problemDef.dissolve().potentialMap());
    for (auto _ : state)
        kernel->totalForces(ppForces, geomForces);
}

template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_TotalInterAtomic(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vector3> forces(cfg->nAtoms());
    auto forceKernel = createForceKernel(problemDef);
    for (auto _ : state)
        forceKernel->totalForces(forces, forces, Kernel::ExcludeGeometric);
}

template <SpeciesType speciesType, SpeciesPopulation population>
static void BM_CalculateForces_TotalForces(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    auto *cfg = problemDef.configuration();
    std::vector<Vector3> forces(cfg->nAtoms());
    auto forceKernel = KernelProducer::forceKernel(cfg, problemDef.dissolve().potentialMap());
    for (auto _ : state)
        forceKernel->totalForces(forces, forces);
}
// small molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, SpeciesType::SmallMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, SpeciesType::SmallMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, SpeciesType::SmallMolecule, SpeciesPopulation::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, SpeciesType::SmallMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, SpeciesType::SmallMolecule, SpeciesPopulation::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, SpeciesType::SmallMolecule, SpeciesPopulation::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// medium molecule benchmarks
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesAngle, SpeciesType::MediumMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesBond, SpeciesType::MediumMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_SpeciesTorsion, SpeciesType::MediumMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalInterAtomic, SpeciesType::MediumMolecule, SpeciesPopulation::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, SpeciesType::MediumMolecule, SpeciesPopulation::Small);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalIntraMolecular, SpeciesType::MediumMolecule, SpeciesPopulation::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateForces_TotalForces, SpeciesType::MediumMolecule, SpeciesPopulation::Small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// framework molecule benchmarks
// BENCHMARK_TEMPLATE(BM_CalculateForces_TotalSpecies, ProblemType::frameworkMolecule, Population::single)
//    ->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();

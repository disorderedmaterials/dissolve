// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/energy/energy.h"
#include "common/problems.h"
#include "kernels/producer.h"
#include <benchmark/benchmark.h>

template <ProblemType problem, Population population>
std::unique_ptr<EnergyKernel> createEnergyKernel(Problem<problem, population> &problemDef)
{

    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    auto *cfg = problemDef.configuration();
    return KernelProducer::energyKernel(cfg, procPool, potentialMap);
}

template <ProblemType problem, Population population> static void BM_CalculateEnergy_AtomicWorldEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    auto &i = problemDef.configuration()->atom(0);
    for (auto _ : state)
        energyKernel->totalEnergy(i);
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_SpeciesInterAtomicEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &usedSpecies = problemDef.configuration()->speciesPopulations();
    auto *species = usedSpecies.back().first;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::pairPotentialEnergy(procPool, species, potentialMap);
}

template <ProblemType problem, Population population> static void BM_CalculateEnergy_MoleculeEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto mol = problemDef.configuration()->molecules().front();
    for (auto _ : state)
    {
        double molecularEnergy = energyKernel->totalEnergy(*mol).total();
        benchmark::DoNotOptimize(molecularEnergy);
    }
}

template <ProblemType problem, Population population> static void BM_CalculateEnergy_MoleculeBondEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &bond = mol->species()->bonds().back();
    for (auto _ : state)
    {
        double energy = bond.energy(1.0);
        benchmark::DoNotOptimize(energy);
    }
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_MoleculeTorsionEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &torsion = mol->species()->torsions().front();
    for (auto _ : state)
    {
        double energy = torsion.energy(109.5);
        benchmark::DoNotOptimize(energy);
    }
}
template <ProblemType problem, Population population>
static void BM_CalculateEnergy_MoleculeAngleEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &angle = mol->species()->angles().front();
    for (auto _ : state)
    {
        double energy = angle.energy(109.5);
        benchmark::DoNotOptimize(energy);
    }
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_TotalIntraMolecularEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::intraMolecularEnergy(procPool, problemDef.configuration(), potentialMap);
}
template <ProblemType problem, Population population>
static void BM_CalculateEnergy_TotalInterAtomicEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::pairPotentialEnergy(procPool, problemDef.configuration(), potentialMap);
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_TotalInterMolecularEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::interMolecularEnergy(procPool, problemDef.configuration(), potentialMap);
}

// Small molecule

// Benchmark energy calculation of single atom with world
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, ProblemType::smallMolecule, Population::small);
// Benchmark energy of single species
BENCHMARK_TEMPLATE(BM_CalculateEnergy_SpeciesInterAtomicEnergy, ProblemType::smallMolecule, Population::small);
// Benchmarking individual molecule calculations
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeBondEnergy, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeAngleEnergy, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
// Benchmark energy calculations of the whole system
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalIntraMolecularEnergy, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterMolecularEnergy, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterAtomicEnergy, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);

// Medium molecule

// Benchmark energy calculation of single atom with world
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, ProblemType::mediumMolecule, Population::small);
// Benchmark energy of single species
BENCHMARK_TEMPLATE(BM_CalculateEnergy_SpeciesInterAtomicEnergy, ProblemType::mediumMolecule, Population::small);
// Benchmarking individual molecule calculations
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeBondEnergy, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeAngleEnergy, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeTorsionEnergy, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, ProblemType::mediumMolecule, Population::medium)
    ->Unit(benchmark::kMillisecond);

// Benchmark energy calculations of the whole system
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalIntraMolecularEnergy, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterMolecularEnergy, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(5);

BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterAtomicEnergy, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(5);

BENCHMARK_MAIN();

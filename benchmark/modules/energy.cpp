// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/energy/energy.h"
#include "common/problems.h"
#include "kernels/producer.h"
#include <benchmark/benchmark.h>

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
std::unique_ptr<EnergyKernel> createEnergyKernel(DissolveBenchmarks::Problem<speciesType, population> &problemDef)
{

    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    auto *cfg = problemDef.configuration();
    return KernelProducer::energyKernel(cfg, procPool, potentialMap);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_AtomicWorldEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    auto &i = problemDef.configuration()->atom(0);
    for (auto _ : state)
        energyKernel->totalEnergy(i);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_SpeciesInterAtomicEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto &usedSpecies = problemDef.configuration()->speciesPopulations();
    auto *species = usedSpecies.back().first;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::pairPotentialEnergy(procPool, species, potentialMap);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_MoleculeEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto mol = problemDef.configuration()->molecules().front();
    for (auto _ : state)
    {
        double molecularEnergy = energyKernel->totalEnergy(*mol).total();
        benchmark::DoNotOptimize(molecularEnergy);
    }
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_MoleculeBondEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &bond = mol->species()->bonds().back();
    for (auto _ : state)
    {
        double energy = bond.energy(1.0);
        benchmark::DoNotOptimize(energy);
    }
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_MoleculeTorsionEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &torsion = mol->species()->torsions().front();
    for (auto _ : state)
    {
        double energy = torsion.energy(109.5);
        benchmark::DoNotOptimize(energy);
    }
}
template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_MoleculeAngleEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    const auto &mol = problemDef.configuration()->molecules().front();
    const auto &angle = mol->species()->angles().front();
    for (auto _ : state)
    {
        double energy = angle.energy(109.5);
        benchmark::DoNotOptimize(energy);
    }
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_TotalIntraMolecularEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::intraMolecularEnergy(procPool, problemDef.configuration(), potentialMap);
}
template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_TotalInterAtomicEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::pairPotentialEnergy(procPool, problemDef.configuration(), potentialMap);
}

template <DissolveBenchmarks::SpeciesType speciesType, DissolveBenchmarks::Population population>
static void BM_CalculateEnergy_TotalInterMolecularEnergy(benchmark::State &state)
{
    DissolveBenchmarks::Problem<speciesType, population> problemDef;
    auto &procPool = problemDef.dissolve().worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve().potentialMap();
    for (auto _ : state)
        EnergyModule::interMolecularEnergy(procPool, problemDef.configuration(), potentialMap);
}

// Small molecule

// Benchmark energy calculation of single atom with world
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
// Benchmark energy of single species
BENCHMARK_TEMPLATE(BM_CalculateEnergy_SpeciesInterAtomicEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
// Benchmarking individual molecule calculations
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeBondEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeAngleEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
// Benchmark energy calculations of the whole system
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalIntraMolecularEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterMolecularEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterAtomicEnergy, DissolveBenchmarks::SpeciesType::SmallMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);

// Medium molecule

// Benchmark energy calculation of single atom with world
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicWorldEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
// Benchmark energy of single species
BENCHMARK_TEMPLATE(BM_CalculateEnergy_SpeciesInterAtomicEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
// Benchmarking individual molecule calculations
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeBondEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeAngleEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeTorsionEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Medium)
    ->Unit(benchmark::kMillisecond);

// Benchmark energy calculations of the whole system
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalIntraMolecularEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterMolecularEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(5);

BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterAtomicEnergy, DissolveBenchmarks::SpeciesType::MediumMolecule,
                   DissolveBenchmarks::Population::Small)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(5);

BENCHMARK_MAIN();

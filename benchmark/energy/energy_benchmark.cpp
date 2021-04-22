// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "benchmark/benchmark.h"
#include "classes/cell.h"
#include "classes/energykernel.h"
#include "classes/species.h"
#include "common/problems.h"

template <ProblemType problem, Population population> EnergyKernel createEnergyKernel(Problem<problem, population> &problemDef)
{

    auto &procPool = problemDef.dissolve_.worldPool();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    auto *cfg = problemDef.cfg_;
    EnergyKernel kernel(procPool, cfg, potentialMap);
    return kernel;
}

template <ProblemType problem, Population population> static void BM_CalculateEnergy_AtomicEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto &cellArray = problemDef.cfg_->cells();
    auto *cellI = cellArray.cell(1);
    auto &atoms = cellI->atoms();
    int numAtoms = atoms.size();
    auto i = atoms[5];
    for (auto _ : state)
        energyKernel.energy(i, cellI, KernelFlags::Flags::NoFlags, ProcessPool::PoolStrategy, false);
}

template <ProblemType problem, Population population> static void BM_CalculateEnergy_MoleculeEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    for (auto _ : state)
    {
        double molecularEnergy = energyKernel.energy(mol, ProcessPool::PoolStrategy, true);
        benchmark::DoNotOptimize(molecularEnergy);
    }
}

template <ProblemType problem, Population population> static void BM_CalculateEnergy_MoleculeBondEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    const auto &bond = mol->species()->bonds().back();
    for (auto _ : state)
    {
        double energy = energyKernel.energy(bond);
        benchmark::DoNotOptimize(energy);
    }
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_MoleculeTorsionEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    const auto &torsion = mol->species()->torsions().front();
    for (auto _ : state)
    {
        double energy = energyKernel.energy(torsion);
        benchmark::DoNotOptimize(energy);
    }
}
template <ProblemType problem, Population population>
static void BM_CalculateEnergy_MoleculeAngleEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto energyKernel = createEnergyKernel(problemDef);
    const auto &mol = problemDef.cfg_->molecules().front();
    const auto &angle = mol->species()->angles().front();
    for (auto _ : state)
    {
        double energy = energyKernel.energy(angle);
        benchmark::DoNotOptimize(energy);
    }
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_TotalIntraMolecularEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &procPool = problemDef.dissolve_.worldPool();
    const auto &cellArray = problemDef.cfg_->cells();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
        EnergyModule::intraMolecularEnergy(procPool, problemDef.cfg_, potentialMap);
}
template <ProblemType problem, Population population>
static void BM_CalculateEnergy_TotalInterAtomicEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &procPool = problemDef.dissolve_.worldPool();
    const auto &cellArray = problemDef.cfg_->cells();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
        EnergyModule::interAtomicEnergy(procPool, problemDef.cfg_, potentialMap);
}

template <ProblemType problem, Population population>
static void BM_CalculateEnergy_TotalInterMolecularEnergy(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    auto &procPool = problemDef.dissolve_.worldPool();
    const auto &cellArray = problemDef.cfg_->cells();
    const PotentialMap &potentialMap = problemDef.dissolve_.potentialMap();
    for (auto _ : state)
        EnergyModule::interMolecularEnergy(procPool, problemDef.cfg_, potentialMap);
}
// Small molecule
// Benchmarking pairwise energy calculation of an individual atom with another cell
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicEnergy, ProblemType::smallMolecule, Population::small);
// Benchmarking individual molecule calculations
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeBondEnergy, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeAngleEnergy, ProblemType::smallMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
// Benchmark energy calculations of the whole system
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalIntraMolecularEnergy, ProblemType::smallMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterMolecularEnergy, ProblemType::smallMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterAtomicEnergy, ProblemType::smallMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);

// medium molecule
// Benchmarking pairwise energy calculation of an individual atom with another cell
BENCHMARK_TEMPLATE(BM_CalculateEnergy_AtomicEnergy, ProblemType::mediumMolecule, Population::small);
// Benchmarking individual molecule calculations
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeBondEnergy, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeAngleEnergy, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeTorsionEnergy, ProblemType::mediumMolecule, Population::small);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_MoleculeEnergy, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
// Benchmark energy calculations of the whole system
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalIntraMolecularEnergy, ProblemType::mediumMolecule, Population::small)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterMolecularEnergy, ProblemType::mediumMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_CalculateEnergy_TotalInterAtomicEnergy, ProblemType::mediumMolecule, Population::small)
    ->Iterations(5)
    ->Unit(benchmark::kMillisecond);
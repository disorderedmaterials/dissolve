// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "math/combinations.h"
#include "nodes/energy/energy.h"
#include "templates/algorithms.h"
#include "templates/parallelDefs.h"
#include <atomic>
#include <numeric>

namespace
{
// Structure to store energy values
struct Energies
{
    double bondEnergy;
    double angleEnergy;
    double torsionEnergy;
    double improperEnergy;

    Energies operator+(const Energies &other) const
    {
        return {.bondEnergy = this->bondEnergy + other.bondEnergy,
                .angleEnergy = this->angleEnergy + other.angleEnergy,
                .torsionEnergy = this->torsionEnergy + other.torsionEnergy,
                .improperEnergy = this->improperEnergy + other.improperEnergy};
    }
};
} // namespace

// Return total pair potential energy of Configuration
PairPotentialEnergyValue EnergyNode::pairPotentialEnergy(const ProcessPool &procPool, const Configuration *cfg,
                                                         const PotentialMap &potentialMap)
{
    /*
     * Calculates the total interatomic energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms, accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    auto kernel = KernelProducer::energyKernel(cfg, procPool, potentialMap);

    // Set the strategy
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Grab the Cell array and calculate total energy
    auto ppEnergy = kernel->totalPairPotentialEnergy(true, strategy);

    // Print process-local energy
    message("Interatomic Energy (Local) is {:15.9e}\n", ppEnergy.total());

    // Sum energy over all processes in the pool and print
    auto interPPEnergy = ppEnergy.interMolecular();
    auto intraPPEnergy = ppEnergy.intraMolecular();
    procPool.allSum(&interPPEnergy, 1, strategy);
    procPool.allSum(&intraPPEnergy, 1, strategy);
    message("Interatomic Energy (World) is {:15.9e}\n", interPPEnergy + intraPPEnergy);

    return {interPPEnergy, intraPPEnergy};
}

// Return total pair potential energy of Species
double EnergyNode::pairPotentialEnergy(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap)
{
    const auto cutoff = potentialMap.range();

    // Get start/end for loop
    Combinations comb(sp->nAtoms());
    auto offset = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto nChunks = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);
    auto [loopStart, loopEnd] = chop_range(0, comb.getNumCombinations(), nChunks, offset);

    double energy = dissolve::transform_reduce(ParallelPolicies::par, dissolve::counting_iterator<int>(loopStart),
                                               dissolve::counting_iterator<int>(loopEnd), 0.0, std::plus<>(),
                                               [&](const auto idx)
                                               {
                                                   auto [n, m] = comb.nthCombination(idx);
                                                   auto &i = sp->atom(n);
                                                   auto &j = sp->atom(m);
                                                   auto &rI = i.r();
                                                   auto &rJ = j.r();

                                                   // Get interatomic distance
                                                   double r = (rJ - rI).magnitude();
                                                   if (r > cutoff)
                                                       return 0.0;

                                                   // Get intramolecular scaling of atom pair
                                                   auto &&[scalingType, elec14, vdw14] = i.scaling(&j);
                                                   if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                                       return potentialMap.energy(&i, &j, r);
                                                   else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                                       return potentialMap.energy(&i, &j, r, elec14, vdw14);

                                                   return 0.0;
                                               });

    return energy;
}

// Return total intermolecular energy of Configuration
double EnergyNode::interMolecularEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap)
{
    /*
     * Calculates the total intermolecular energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms of different Molecules, thus neglecting intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    auto kernel = KernelProducer::energyKernel(cfg, procPool, potentialMap);

    // Set the strategy
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Grab the Cell array and calculate total energy
    auto ppEnergy = kernel->totalPairPotentialEnergy(false, strategy).total();

    // Print process-local energy
    message("Intermolecular Energy (Local) is {:15.9e}\n", ppEnergy);

    // Sum energy over all processes in the pool and print
    procPool.allSum(&ppEnergy, 1, strategy);
    message("Intermolecular Energy (World) is {:15.9e}\n", ppEnergy);

    return ppEnergy;
}

// Return total intramolecular energy of Configuration
double EnergyNode::intraMolecularEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap)
{
    double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;

    return intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);
}

// Return total intramolecular energy of Configuration, storing components in provided variables
double EnergyNode::intraMolecularEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                                        double &bondEnergy, double &angleEnergy, double &torsionEnergy, double &improperEnergy)
{
    /*
     * Calculate the total intramolecular energy of the system, arising from Bond, Angle, and Torsion
     * terms in all Molecules.
     *
     * This is a parallel routine, with processes operating as a standard world group.
     */

    // Create an EnergyKernel
    auto kernel = KernelProducer::energyKernel(cfg, procPool, potentialMap);

    bondEnergy = 0;
    angleEnergy = 0;
    torsionEnergy = 0;
    improperEnergy = 0;

    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);

    const auto &molecules = cfg->molecules();
    std::shared_ptr<const Molecule> mol;
    auto [begin, end] = chop_range(molecules.begin(), molecules.end(), stride, start);

    auto unaryOp = [&](const auto &mol) -> Energies
    {
        Energies localEnergies{.bondEnergy = 0.0, .angleEnergy = 0.0, .torsionEnergy = 0.0, .improperEnergy = 0.0};

        // Loop over Bond
        localEnergies.bondEnergy +=
            std::accumulate(mol->species()->bonds().cbegin(), mol->species()->bonds().cend(), 0.0,
                            [&mol, &kernel](auto const acc, const auto &t)
                            { return acc + kernel->bondEnergy(t, *mol->atom(t.indexI()), *mol->atom(t.indexJ())); });

        // Loop over Angle
        localEnergies.angleEnergy += std::accumulate(
            mol->species()->angles().cbegin(), mol->species()->angles().cend(), 0.0,
            [&mol, &kernel](auto const acc, const auto &t)
            { return acc + kernel->angleEnergy(t, *mol->atom(t.indexI()), *mol->atom(t.indexJ()), *mol->atom(t.indexK())); });

        // Loop over Torsions
        localEnergies.torsionEnergy +=
            std::accumulate(mol->species()->torsions().cbegin(), mol->species()->torsions().cend(), 0.0,
                            [&mol, &kernel](auto const acc, const auto &t)
                            {
                                return acc + kernel->torsionEnergy(t, *mol->atom(t.indexI()), *mol->atom(t.indexJ()),
                                                                   *mol->atom(t.indexK()), *mol->atom(t.indexL()));
                            });

        localEnergies.improperEnergy +=
            std::accumulate(mol->species()->impropers().cbegin(), mol->species()->impropers().cend(), 0.0,
                            [&mol, &kernel](auto const acc, const auto &imp)
                            {
                                return acc + kernel->improperEnergy(imp, *mol->atom(imp.indexI()), *mol->atom(imp.indexJ()),
                                                                    *mol->atom(imp.indexK()), *mol->atom(imp.indexL()));
                            });

        return localEnergies;
    };

    auto energies = dissolve::transform_reduce(ParallelPolicies::par, begin, end, Energies(), std::plus<Energies>(), unaryOp);

    bondEnergy = energies.bondEnergy;
    angleEnergy = energies.angleEnergy;
    improperEnergy = energies.improperEnergy;
    torsionEnergy = energies.torsionEnergy;
    double totalIntra = bondEnergy + angleEnergy + torsionEnergy + improperEnergy;

    message("Intramolecular Energy (Local) is {:15.9e} kJ/mol ({:15.9e} bond + {:15.9e} angle + {:15.9e} "
            "torsion + {:15.9e} improper)\n",
            totalIntra, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    // Sum energy and print
    double values[4];
    values[0] = bondEnergy;
    values[1] = angleEnergy;
    values[2] = torsionEnergy;
    values[3] = improperEnergy;
    procPool.allSum(values, 4, strategy);
    bondEnergy = values[0];
    angleEnergy = values[1];
    torsionEnergy = values[2];
    improperEnergy = values[3];

    totalIntra = bondEnergy + angleEnergy + torsionEnergy + improperEnergy;

    message("Intramolecular Energy (World) is {:15.9e} kJ/mol ({:15.9e} bond + {:15.9e} angle + {:15.9e} "
            "torsion + {:15.9e} improper)\n",
            totalIntra, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    return totalIntra;
}

// Return total intramolecular energy of Species
double EnergyNode::intraMolecularEnergy(const Species *sp)
{
    auto energy = 0.0;
    const Box *box = sp->box();

    // Loop over bonds
    energy += std::accumulate(sp->bonds().begin(), sp->bonds().end(), 0.0,
                              [box](const auto acc, const auto &b)
                              { return acc + b.energy(box->minimumDistance(b.j()->r(), b.i()->r())); });

    // Loop over angles
    energy += std::accumulate(sp->angles().begin(), sp->angles().end(), 0.0,
                              [box](const auto acc, const auto &a)
                              { return acc + a.energy(box->angleInRadians(a.i()->r(), a.j()->r(), a.k()->r())); });

    // Loop over torsions
    energy += std::accumulate(sp->torsions().begin(), sp->torsions().end(), 0.0,
                              [box](const auto acc, const auto &t) {
                                  return acc + t.energy(box->torsionInRadians(t.i()->r(), t.j()->r(), t.k()->r(), t.l()->r()));
                              });

    // Loop over impropers
    energy += std::accumulate(
        sp->impropers().begin(), sp->impropers().end(), 0.0,
        [box](const auto acc, const auto &imp)
        { return acc + imp.energy(box->torsionInRadians(imp.i()->r(), imp.j()->r(), imp.k()->r(), imp.l()->r())); });

    return energy;
}

// Return total energy (interatomic and intramolecular) of Configuration
double EnergyNode::totalEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap)
{
    return (pairPotentialEnergy(procPool, cfg, potentialMap).total() + intraMolecularEnergy(procPool, cfg, potentialMap));
}

// Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
double EnergyNode::totalEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                               PairPotentialEnergyValue &interEnergy, double &bondEnergy, double &angleEnergy,
                               double &torsionEnergy, double &improperEnergy)
{
    interEnergy = pairPotentialEnergy(procPool, cfg, potentialMap);
    intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    return interEnergy.total() + bondEnergy + angleEnergy + torsionEnergy + improperEnergy;
}

// Return total energy (interatomic and intramolecular) of Species
double EnergyNode::totalEnergy(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap)
{
    return (pairPotentialEnergy(procPool, sp, potentialMap) + intraMolecularEnergy(sp));
}

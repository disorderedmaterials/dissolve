// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "modules/energy/energy.h"
#include "templates/algorithms.h"
#include <numeric>

// Return total interatomic energy of Configuration
double EnergyModule::interAtomicEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    /*
     * Calculates the total interatomic energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms, accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    EnergyKernel kernel(procPool, cfg, potentialMap);

    // Set the strategy
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Grab the Cell array and calculate total energy
    const auto &cellArray = cfg->cells();
    double totalEnergy = kernel.energy(cellArray, false, strategy, false);

    // Print process-local energy
    Messenger::printVerbose("Interatomic Energy (Local) is {:15.9e}\n", totalEnergy);

    // Sum energy over all processes in the pool and print
    procPool.allSum(&totalEnergy, 1, strategy);
    Messenger::printVerbose("Interatomic Energy (World) is {:15.9e}\n", totalEnergy);

    return totalEnergy;
}

// Return total interatomic energy of Species
double EnergyModule::interAtomicEnergy(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap)
{
    Vec3<double> rI;
    double r, scale, energy = 0.0;
    const auto cutoff = potentialMap.range();

    // Get start/end for loop
    auto loopStart = procPool.twoBodyLoopStart(sp->nAtoms());
    auto loopEnd = procPool.twoBodyLoopEnd(sp->nAtoms());

    // Double loop over species atoms
    // NOTE PR #334 : use for_each_pair
    for (auto indexI = loopStart; indexI <= loopEnd; ++indexI)
    {
        auto &i = sp->atom(indexI);
        rI = i.r();

        for (auto indexJ = indexI + 1; indexJ < sp->nAtoms(); ++indexJ)
        {
            auto &j = sp->atom(indexJ);

            // Get interatomic distance
            r = (j.r() - rI).magnitude();
            if (r > cutoff)
                continue;

            // Get intramolecular scaling of atom pair
            scale = i.scaling(&j);
            if (scale < 1.0e-3)
                continue;

            energy += potentialMap.energy(&i, &j, r) * scale;
        }
    }

    return energy;
}

// Return total intermolecular energy of Configuration
double EnergyModule::interMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    /*
     * Calculates the total intermolecular energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms of different Molecules, thus neglecting intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    EnergyKernel kernel(procPool, cfg, potentialMap);

    // Set the strategy
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Grab the Cell array and calculate total energy
    const auto &cellArray = cfg->cells();
    double totalEnergy = kernel.energy(cellArray, true, strategy, false);

    // Print process-local energy
    Messenger::printVerbose("Intermolecular Energy (Local) is {:15.9e}\n", totalEnergy);

    // Sum energy over all processes in the pool and print
    procPool.allSum(&totalEnergy, 1, strategy);
    Messenger::printVerbose("Intermolecular Energy (World) is {:15.9e}\n", totalEnergy);

    return totalEnergy;
}

// Return total intramolecular energy of Configuration
double EnergyModule::intraMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;

    return intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);
}

// Return total intramolecular energy of Configuration, storing components in provided variables
double EnergyModule::intraMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                          double &bondEnergy, double &angleEnergy, double &torsionEnergy,
                                          double &improperEnergy)
{
    /*
     * Calculate the total intramolecular energy of the system, arising from Bond, Angle, and Torsion
     * terms in all Molecules.
     *
     * This is a parallel routine, with processes operating as a standard world group.
     */

    // Create an EnergyKernel
    EnergyKernel kernel(procPool, cfg, potentialMap);

    bondEnergy = 0;
    angleEnergy = 0;
    torsionEnergy = 0;
    improperEnergy = 0;

    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);

    std::deque<std::shared_ptr<Molecule>> molecules = cfg->molecules();
    std::shared_ptr<const Molecule> mol;
    auto [begin, end] = chop_range(cfg->molecules().begin(), cfg->molecules().end(), stride, start);
    for (auto it = begin; it < end; ++it)
    {
        // Get Molecule pointer
        mol = *it;

        // Loop over Bond
        bondEnergy += std::accumulate(mol->species()->bonds().cbegin(), mol->species()->bonds().cend(), 0.0,
                                      [&mol, &kernel](auto const acc, const auto &t) {
                                          return acc + kernel.energy(t, mol->atom(t.indexI()), mol->atom(t.indexJ()));
                                      });

        // Loop over Angle
        angleEnergy += std::accumulate(mol->species()->angles().cbegin(), mol->species()->angles().cend(), 0.0,
                                       [&mol, &kernel](auto const acc, const auto &t) {
                                           return acc + kernel.energy(t, mol->atom(t.indexI()), mol->atom(t.indexJ()),
                                                                      mol->atom(t.indexK()));
                                       });

        // Loop over Torsions
        torsionEnergy += std::accumulate(mol->species()->torsions().cbegin(), mol->species()->torsions().cend(), 0.0,
                                         [&mol, &kernel](auto const acc, const auto &t) {
                                             return acc + kernel.energy(t, mol->atom(t.indexI()), mol->atom(t.indexJ()),
                                                                        mol->atom(t.indexK()), mol->atom(t.indexL()));
                                         });

        improperEnergy += std::accumulate(mol->species()->impropers().cbegin(), mol->species()->impropers().cend(), 0.0,
                                          [&mol, &kernel](auto const acc, const auto &imp) {
                                              return acc + kernel.energy(imp, mol->atom(imp.indexI()), mol->atom(imp.indexJ()),
                                                                         mol->atom(imp.indexK()), mol->atom(imp.indexL()));
                                          });
    }

    double totalIntra = bondEnergy + angleEnergy + torsionEnergy + improperEnergy;

    Messenger::printVerbose("Intramolecular Energy (Local) is {:15.9e} kJ/mol ({:15.9e} bond + {:15.9e} angle + {:15.9e} "
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

    Messenger::printVerbose("Intramolecular Energy (World) is {:15.9e} kJ/mol ({:15.9e} bond + {:15.9e} angle + {:15.9e} "
                            "torsion + {:15.9e} improper)\n",
                            totalIntra, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    return totalIntra;
}

// Return total intramolecular energy of Species
double EnergyModule::intraMolecularEnergy(ProcessPool &procPool, Species *sp)
{
    auto energy = 0.0;

    // Loop over bonds
    for (const auto &bond : sp->bonds())
        energy += EnergyKernel::energy(bond);

    // Loop over angles
    for (const auto &angle : sp->angles())
        energy += EnergyKernel::energy(angle);

    // Loop over torsions
    for (const auto &torsion : sp->torsions())
        energy += EnergyKernel::energy(torsion);

    // Loop over impropers
    for (const auto &improper : sp->impropers())
        energy += EnergyKernel::energy(improper);

    return energy;
}

// Return total energy (interatomic and intramolecular) of Configuration
double EnergyModule::totalEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap)
{
    return (interAtomicEnergy(procPool, cfg, potentialMap) + intraMolecularEnergy(procPool, cfg, potentialMap));
}

// Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
double EnergyModule::totalEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                 double &interEnergy, double &bondEnergy, double &angleEnergy, double &torsionEnergy,
                                 double &improperEnergy)
{
    interEnergy = interAtomicEnergy(procPool, cfg, potentialMap);
    intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    return interEnergy + bondEnergy + angleEnergy + torsionEnergy + improperEnergy;
}

// Return total energy (interatomic and intramolecular) of Species
double EnergyModule::totalEnergy(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap)
{
    return (interAtomicEnergy(procPool, sp, potentialMap) + intraMolecularEnergy(procPool, sp));
}

// Check energy stability of specified Configuration
EnergyModule::EnergyStability EnergyModule::checkStability(GenericList &processingData, Configuration *cfg)
{
    // First, check if the Configuration is targetted by an EnergyModule
    if (!processingData.valueOr<bool>("IsEnergyModuleTarget", cfg->niceName(), false))
    {
        Messenger::error("Configuration '{}' is not targeted by any EnergyModule, so stability cannot be assessed. "
                         "Check your setup!\n",
                         cfg->name());
        return EnergyModule::NotAssessable;
    }

    // Retrieve the EnergyStable flag from the Configuration's module data
    if (processingData.contains("EnergyStable", cfg->niceName()))
    {
        auto stable = processingData.value<bool>("EnergyStable", cfg->niceName());
        if (!stable)
        {
            Messenger::print("Energy for Configuration '{}' is not yet stable.\n", cfg->name());
            return EnergyModule::EnergyUnstable;
        }
    }
    else
    {
        Messenger::warn("No energy stability information is present for Configuration '{}' (yet) - check your setup.\n",
                        cfg->name());
        return EnergyModule::NotAssessable;
    }

    return EnergyModule::EnergyStable;
}

// Check energy stability of specified Configurations, returning the number that failed
int EnergyModule::nUnstable(GenericList &processingData, const RefList<Configuration> &configurations)
{
    auto nFailed = 0;

    for (auto *cfg : configurations)
    {
        // Check the stability of this Configuration
        auto result = checkStability(processingData, cfg);

        if (result == EnergyModule::EnergyStable)
            ++nFailed;
        else if (result == EnergyModule::NotAssessable)
            return EnergyModule::NotAssessable;
    }

    return nFailed;
}

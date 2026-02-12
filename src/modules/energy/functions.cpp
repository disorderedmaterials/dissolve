// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "math/combinations.h"
#include "modules/energy/energy.h"
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
PairPotentialEnergyValue EnergyModule::pairPotentialEnergy(const Configuration *cfg, const PotentialMap &potentialMap)
{
    // Create an EnergyKernel
    auto kernel = KernelProducer::energyKernel(cfg, potentialMap);

    // Calculate total energy
    auto ppEnergy = kernel->totalPairPotentialEnergy(true);

    Messenger::printVerbose("Interatomic energy is {:15.9e}\n", ppEnergy.total());

    return ppEnergy;
}

// Return total pair potential energy of Species
double EnergyModule::pairPotentialEnergy(const Species *sp, const PotentialMap &potentialMap)
{
    const auto cutoff = potentialMap.range();

    Combinations comb(sp->nAtoms());
    return dissolve::transform_reduce(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
                                      dissolve::counting_iterator<int>(comb.getNumCombinations()), 0.0, std::plus<>(),
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
}

// Return total intermolecular energy of Configuration
double EnergyModule::interMolecularEnergy(const Configuration *cfg, const PotentialMap &potentialMap)
{
    /*
     * Calculates the total intermolecular energy of the system, i.e. the energy contributions from PairPotential
     * interactions between individual Atoms of different Molecules, thus neglecting intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create an EnergyKernel
    auto kernel = KernelProducer::energyKernel(cfg, potentialMap);

    // Grab the Cell array and calculate total energy
    auto ppEnergy = kernel->totalPairPotentialEnergy(false).total();

    Messenger::printVerbose("Intermolecular energy is {:15.9e}\n", ppEnergy);

    return ppEnergy;
}

// Return total intramolecular energy of Configuration
double EnergyModule::intraMolecularEnergy(const Configuration *cfg, const PotentialMap &potentialMap)
{
    double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;

    return intraMolecularEnergy(cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);
}

// Return total intramolecular energy of Configuration, storing components in provided variables
double EnergyModule::intraMolecularEnergy(const Configuration *cfg, const PotentialMap &potentialMap, double &bondEnergy,
                                          double &angleEnergy, double &torsionEnergy, double &improperEnergy)
{
    /*
     * Calculate the total intramolecular energy of the system, arising from Bond, Angle, and Torsion
     * terms in all Molecules.
     *
     * This is a parallel routine, with processes operating as a standard world group.
     */

    // Create an EnergyKernel
    auto kernel = KernelProducer::energyKernel(cfg, potentialMap);

    bondEnergy = 0;
    angleEnergy = 0;
    torsionEnergy = 0;
    improperEnergy = 0;

    const auto &molecules = cfg->molecules();

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

    auto energies = dissolve::transform_reduce(ParallelPolicies::par, molecules.begin(), molecules.end(), Energies(),
                                               std::plus<Energies>(), unaryOp);

    bondEnergy = energies.bondEnergy;
    angleEnergy = energies.angleEnergy;
    improperEnergy = energies.improperEnergy;
    torsionEnergy = energies.torsionEnergy;
    double totalIntra = bondEnergy + angleEnergy + torsionEnergy + improperEnergy;

    Messenger::printVerbose("Intramolecular energy is {:15.9e} kJ/mol ({:15.9e} bond + {:15.9e} angle + {:15.9e} "
                            "torsion + {:15.9e} improper)\n",
                            totalIntra, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    return totalIntra;
}

// Return total intramolecular energy of Species
double EnergyModule::intraMolecularEnergy(const Species *sp)
{
    auto energy = 0.0;
    const Box *box = sp->box();

    // Loop over bonds
    energy += std::accumulate(sp->bonds().begin(), sp->bonds().end(), 0.0, [box](const auto acc, const auto &b)
                              { return acc + b.energy(box->minimumDistance(b.j()->r(), b.i()->r())); });

    // Loop over angles
    energy += std::accumulate(sp->angles().begin(), sp->angles().end(), 0.0, [box](const auto acc, const auto &a)
                              { return acc + a.energy(box->angleInRadians(a.i()->r(), a.j()->r(), a.k()->r())); });

    // Loop over torsions
    energy +=
        std::accumulate(sp->torsions().begin(), sp->torsions().end(), 0.0, [box](const auto acc, const auto &t)
                        { return acc + t.energy(box->torsionInRadians(t.i()->r(), t.j()->r(), t.k()->r(), t.l()->r())); });

    // Loop over impropers
    energy += std::accumulate(
        sp->impropers().begin(), sp->impropers().end(), 0.0, [box](const auto acc, const auto &imp)
        { return acc + imp.energy(box->torsionInRadians(imp.i()->r(), imp.j()->r(), imp.k()->r(), imp.l()->r())); });

    return energy;
}

// Return total energy (interatomic and intramolecular) of Configuration
double EnergyModule::totalEnergy(const Configuration *cfg, const PotentialMap &potentialMap)
{
    return (pairPotentialEnergy(cfg, potentialMap).total() + intraMolecularEnergy(cfg, potentialMap));
}

// Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
double EnergyModule::totalEnergy(const Configuration *cfg, const PotentialMap &potentialMap,
                                 PairPotentialEnergyValue &interEnergy, double &bondEnergy, double &angleEnergy,
                                 double &torsionEnergy, double &improperEnergy)
{
    interEnergy = pairPotentialEnergy(cfg, potentialMap);
    intraMolecularEnergy(cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    return interEnergy.total() + bondEnergy + angleEnergy + torsionEnergy + improperEnergy;
}

// Return total energy (interatomic and intramolecular) of Species
double EnergyModule::totalEnergy(const Species *sp, const PotentialMap &potentialMap)
{
    return (pairPotentialEnergy(sp, potentialMap) + intraMolecularEnergy(sp));
}

// Check energy stability of specified Configuration
EnergyModule::EnergyStability EnergyModule::checkStability(GenericList &processingData, const Configuration *cfg)
{
    // First, check if the Configuration is targetted by an EnergyModule
    if (!processingData.valueOr<bool>("IsEnergyModuleTarget", cfg->name(), false))
    {
        Messenger::error("Configuration '{}' is not targeted by any EnergyModule, so stability cannot be assessed. "
                         "Check your setup!\n",
                         cfg->name());
        return EnergyModule::NotAssessable;
    }

    // Retrieve the EnergyStable flag from the Configuration's module data
    if (processingData.contains("EnergyStable", cfg->name()))
    {
        auto stable = processingData.value<bool>("EnergyStable", cfg->name());
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

// Check energy stability of specified Configurations, returning the number that ExecutionResult::Failed
int EnergyModule::nUnstable(GenericList &processingData, const std::vector<Configuration *> &configurations)
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

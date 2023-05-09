// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/externalPotentials.h"
#include "classes/configuration.h"
#include <numeric>

/*
 * Energy
 */

ExternalPotentialsEnergyKernel::ExternalPotentialsEnergyKernel(const Configuration *cfg, const ProcessPool &procPool,
                                                               const PotentialMap &potentialMap,
                                                               std::optional<double> energyCutoff)
    : EnergyKernel(cfg, procPool, potentialMap, energyCutoff), globalPotentials_(cfg->globalPotentials())
{
}

// Return external energy of supplied atom
double ExternalPotentialsEnergyKernel::extendedEnergy(const Atom &i) const
{
    return std::accumulate(globalPotentials_.begin(), globalPotentials_.end(), 0.0,
                           [&](const auto acc, const auto &pot) { return acc + pot->energy(i, box_); });
}

// Return external energy of supplied molecule
double ExternalPotentialsEnergyKernel::extendedEnergy(const Molecule &mol) const
{
    return std::accumulate(globalPotentials_.begin(), globalPotentials_.end(), 0.0,
                           [&](const auto outerAcc, const auto &pot)
                           {
                               return outerAcc + std::accumulate(mol.atoms().begin(), mol.atoms().end(), 0.0,
                                                                 [&](const auto innerAcc, const auto &i)
                                                                 { return innerAcc + pot->energy(*i, box_); });
                           });
}

/*
 * Force
 */

ExternalPotentialsForceKernel::ExternalPotentialsForceKernel(const Configuration *cfg, const ProcessPool &procPool,
                                                             const PotentialMap &potentialMap,
                                                             std::optional<double> energyCutoff)
    : ForceKernel(cfg, procPool, potentialMap, energyCutoff), globalPotentials_(cfg->globalPotentials())
{
}

// Calculate extended forces on supplied molecule
void ExternalPotentialsForceKernel::extendedForces(const Molecule &mol, ForceVector &f) const
{
    for (const auto &pot : globalPotentials_)
        for (const auto &i : mol.atoms())
            pot->force(*i, box_, f[i->arrayIndex()]);
}

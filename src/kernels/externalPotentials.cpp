// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
                           [&](const auto acc, const auto &pot) { return acc + pot->energy(i, box_); }) +
           std::accumulate(i.targetedPotentials().begin(), i.targetedPotentials().end(), 0.0,
                           [&](const auto acc, const auto &pot) { return acc + pot->energy(i, box_); });
}

// Return external energy of supplied molecule
double ExternalPotentialsEnergyKernel::extendedEnergy(const Molecule &mol) const
{
    return std::accumulate(mol.atoms().begin(), mol.atoms().end(), 0.0,
                           [&](const auto acc, const auto &i) { return acc + extendedEnergy(*i); });
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

// Calculate extended forces on supplied atom
void ExternalPotentialsForceKernel::extendedForces(const Atom &i, Vec3<double> &fVec) const
{
    for (const auto &pot : globalPotentials_)
        pot->force(i, box_, fVec);
    for (const auto &pot : i.targetedPotentials())
        pot->force(i, box_, fVec);
}

// Calculate extended forces on supplied molecule
void ExternalPotentialsForceKernel::extendedForces(const Molecule &mol, ForceVector &f) const
{
    auto offset = mol.globalAtomOffset();
    for (const auto &i : mol.atoms())
        extendedForces(*i, f[offset++]);
}

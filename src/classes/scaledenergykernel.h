// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/energykernel.h"

// Forward Declarations
/* none */

// Scaled Energy Kernel
class ScaledEnergyKernel : public EnergyKernel
{
    public:
    ScaledEnergyKernel(double interMoleculeRScale, double intraMoleculeEScale, ProcessPool &procPool, Configuration *config,
                       const PotentialMap &potentialMap, double energyCutoff = -1.0);
    ~ScaledEnergyKernel();
    // Clear all data
    void clear();

    /*
     * Scale Factor
     */
    private:
    // Scale factor to apply to Molecule distances used when calculating interatomic PairPotential energies
    double interMoleculeRScale_;
    // Scale factor to apply to intermolecular PairPotential energies arising from interatomic interactions within the same
    // Molecule
    double intraMoleculeEScale_;

    /*
     * Internal Routines
     */
    private:
    // Return PairPotential energy between atoms provided as pointers, at the distance specified
    double pairPotentialEnergy(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double r);
};

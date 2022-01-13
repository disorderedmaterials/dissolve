// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/energykernel.h"

// Scaled Energy Kernel
class ScaledEnergyKernel : public EnergyKernel
{
    public:
    ScaledEnergyKernel(double interMoleculeRScale, double intraMoleculeEScale, ProcessPool &procPool, const Box *box,
                       const CellArray &cells, const PotentialMap &potentialMap, double energyCutoff = -1.0);
    ~ScaledEnergyKernel() = default;
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
    double pairPotentialEnergy(const Atom &i, const Atom &j, double r) override;
};

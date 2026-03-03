// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/energy.h"
#include "kernels/force.h"

// Forward Declarations
class ExternalPotential;

// Energy Kernel with External Potentials
class ExternalPotentialsEnergyKernel : public EnergyKernel
{
    private:
    friend class KernelProducer;
    ExternalPotentialsEnergyKernel(const Configuration *cfg, const PotentialMap &potentialMap);

    public:
    ~ExternalPotentialsEnergyKernel() = default;

    /*
     * Source External Potentials
     */
    private:
    // Global potentials acting on all atoms
    const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials_;

    /*
     * Extended Terms
     */
    private:
    // Return extended energy of supplied atom
    double extendedEnergy(const Atom &i) const override;
    // Return extended energy of supplied molecule
    double extendedEnergy(const Molecule &mol) const override;
};

// Force Kernel with External Potentials
class ExternalPotentialsForceKernel : public ForceKernel
{
    private:
    friend class KernelProducer;
    ExternalPotentialsForceKernel(const Configuration *cfg, const PotentialMap &potentialMap);

    public:
    ~ExternalPotentialsForceKernel() = default;

    /*
     * Source External Potentials
     */
    private:
    // Global potentials acting on all atoms
    const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials_;

    /*
     * Extended Terms
     */
    private:
    // Calculate extended forces on supplied atom
    void extendedForces(const Atom &i, Vector3 &fVec) const override;
    // Calculate extended forces on supplied molecule
    void extendedForces(const Molecule &mol, ForceVector &f) const override;
};

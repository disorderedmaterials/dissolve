// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "kernels/energy.h"

// Forward Declarations
class ExternalPotential;

// Energy Kernel with External Potentials
class ExternalPotentialsEnergyKernel : public EnergyKernel
{
    private:
    friend class KernelProducer;
    ExternalPotentialsEnergyKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                                   std::optional<double> energyCutoff = {});

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

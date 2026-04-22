// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/common.h"
#include "kernels/geometry.h"
#include "templates/flags.h"

// Forward Declarations
class ConfigurationAtom;
class Cell;
class Species;
class SpeciesAtom;

// Species Kernel
class SpeciesKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    SpeciesKernel(const Species *sp, const PotentialMap &potentialMap);

    public:
    ~SpeciesKernel() = default;

    private:
    // Target species
    const Species *species_{nullptr};

    /*
     * Components
     */
    private:
    // Return pair potential energy of Species
    Kernel::PairPotentialEnergyValue pairPotentialEnergy() const;
    // Return geometric energy of Species
    Kernel::GeometryEnergyValue geometricEnergy() const;
    // Calculate pair potential forces within of Species
    void pairPotentialForces(std::vector<Vector3> &forces) const;
    // Calculate pair potential forces within of Species at the specified coordinates
    void pairPotentialForces(std::vector<Vector3> &forces, const std::vector<Vector3> &r) const;
    // Calculate geometric forces within Species
    void geometricForces(std::vector<Vector3> &forces) const;
    // Calculate geometric forces within Species at the specified coordinates
    void geometricForces(std::vector<Vector3> &forces, const std::vector<Vector3> &r) const;

    /*
     * Totals
     */
    public:
    // Return total energy (interatomic and intramolecular) of Species
    Kernel::EnergyResult totalEnergy(Flags<Kernel::CalculationFlags> flags = {});
    // Calculate total forces within the specified Species
    void totalForces(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometricForceVector,
                     Flags<Kernel::CalculationFlags> flags = {});
    // Calculate total forces within the specified Species at the specified coordinates
    void totalForces(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometricForceVector,
                     const std::vector<Vector3> &r, Flags<Kernel::CalculationFlags> flags = {});
};

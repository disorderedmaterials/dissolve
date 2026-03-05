// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/common.h"
#include "kernels/geometry.h"
#include "templates/flags.h"

// Forward Declarations
class Atom;
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

    /*
     * Components
     */
    private:
    // Return pair potential energy of Species
    Kernel::PairPotentialEnergyValue pairPotentialEnergy(const Species *sp) const;
    // Return geometric energy of Species
    Kernel::GeometryEnergyValue geometricEnergy(const Species *sp) const;
    // Calculate pair potential forces within of Species
    void pairPotentialForces(const Species *sp, std::vector<Vector3> &forces) const;
    // Calculate pair potential forces within of Species at the specified coordinates
    void pairPotentialForces(const Species *sp, std::vector<Vector3> &forces, const std::vector<Vector3> &r) const;
    // Calculate geometric forces within Species
    void geometricForces(const Species *sp, std::vector<Vector3> &forces) const;
    // Calculate geometric forces within Species at the specified coordinates
    void geometricForces(const Species *sp, std::vector<Vector3> &forces, const std::vector<Vector3> &r) const;

    /*
     * Totals
     */
    public:
    // Return total energy (interatomic and intramolecular) of Species
    Kernel::EnergyResult totalEnergy(const Species *sp, Flags<Kernel::CalculationFlags> flags = {});
    // Calculate total forces within the specified Species
    void totalForces(const Species *sp, std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometricForceVector,
                     Flags<Kernel::CalculationFlags> flags = {});
    // Calculate total forces within the specified Species at the specified coordinates
    void totalForces(const Species *sp, std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometricForceVector,
                     const std::vector<Vector3> &r, Flags<Kernel::CalculationFlags> flags = {});
};

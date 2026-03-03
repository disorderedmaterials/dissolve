// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/geometry.h"
#include "kernels/result.h"
#include "templates/flags.h"
#include <memory>
#include <optional>

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
     * Base Routines
     */
    private:
    // Return PairPotential energy between atoms
    double pairPotentialEnergy(const SpeciesAtom *i, const SpeciesAtom *j, double r) const;
    // Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
    double pairPotentialEnergy(const SpeciesAtom *i, const SpeciesAtom *j, double r, double elecScale, double srScale) const;

    /*
     * Totals
     */
    public:
    // Energy calculation flags
    enum EnergyCalculationFlags
    {
        ExcludePairPotential,
        ExcludeGeometry,
        ExcludeExtended,
        ExcludeIntraMolecularPairPotential
    };

    public:
    // Return total interatomic PairPotential energy of the world
    PairPotentialEnergyValue totalPairPotentialEnergy(bool includeIntraMolecular) const;
    // Return total interatomic PairPotential energy from summation of molecules
    PairPotentialEnergyValue totalMoleculePairPotentialEnergy(bool includeIntraMolecular) const;
};

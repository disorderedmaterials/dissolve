// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/common.h"
#include "kernels/geometry.h"
#include "templates/flags.h"

// Forward Declarations
class Atom;
class Cell;
class Configuration;
class Molecule;

// Standard Energy Kernel, inheriting GeometryKernel
class EnergyKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    friend class ExternalPotentialsEnergyKernel;
    EnergyKernel(const Configuration *cfg, const PotentialMap &potentialMap);

    public:
    ~EnergyKernel() = default;

    private:
    // Target Configuration
    const Configuration *configuration_{nullptr};

    /*
     * Base Routines
     */
    private:
    // Return PairPotential energy between atoms
    virtual double pairPotentialEnergy(const Atom &i, const Atom &j, double r) const;
    // Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
    virtual double pairPotentialEnergy(const Atom &i, const Atom &j, double r, double elecScale, double srScale) const;

    /*
     * PairPotential Terms
     */
    private:
    // Return PairPotential energy of atoms in the supplied cell
    Kernel::PairPotentialEnergyValue cellEnergy(const Cell &cell, bool includeInter = true, bool includeIntra = true) const;
    // Return PairPotential energy between two cells
    Kernel::PairPotentialEnergyValue cellToCellEnergy(const Cell &cell, const Cell &otherCell, bool applyMim,
                                                      bool includeInter = true, bool includeIntra = true) const;
    // Return PairPotential energy of atom with world
    double pairPotentialEnergy(const Atom &i) const;
    // Return PairPotential energy of Molecule with world
    Kernel::PairPotentialEnergyValue pairPotentialEnergy(const Molecule &mol, bool includeInter = true,
                                                         bool includeIntra = true) const;

    /*
     * Extended Terms
     */
    private:
    // Return total extended energy
    double totalExtendedEnergy() const;
    // Return energy of supplied atom from ad hoc extended terms
    virtual double extendedEnergy(const Atom &i) const;
    // Return energy of supplied molecule from ad hoc extended terms
    virtual double extendedEnergy(const Molecule &mol) const;

    /*
     * Totals
     */
    public:
    // Return total interatomic PairPotential energy
    Kernel::PairPotentialEnergyValue totalPairPotentialEnergy(bool includeInter = true, bool includeIntra = true) const;
    // Return total interatomic PairPotential energy from summation of molecules
    Kernel::PairPotentialEnergyValue totalMoleculePairPotentialEnergy(bool includeInter = true, bool includeIntra = true) const;
    // Return total energy of supplied atom
    Kernel::EnergyResult totalEnergy(Flags<Kernel::CalculationFlags> flags = {}) const;
    // Return total energy of supplied atom
    Kernel::EnergyResult totalEnergy(const Atom &i) const;
    // Return total energy of supplied molecule
    Kernel::EnergyResult totalEnergy(const Molecule &mol, Flags<Kernel::CalculationFlags> flags = {}) const;
    // Return potential map
    const PotentialMap &potentialMap() const;
    // Calculate energy components with simple double-loops for testing
    Kernel::EnergyResult totalEnergySimple() const;
};

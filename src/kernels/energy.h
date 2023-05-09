// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "kernels/geometry.h"
#include "templates/flags.h"
#include <memory>
#include <optional>

// Forward Declarations
class Atom;
class Cell;
class CellArray;
class Box;
class Configuration;
class PotentialMap;
class Molecule;
class SpeciesBond;
class SpeciesAngle;
class SpeciesImproper;
class SpeciesTorsion;

// Energy Result
class EnergyResult
{
    public:
    EnergyResult(double pp = 0.0, double geom = 0.0, double ext = 0.0)
        : total_(pp + geom + ext), pairPotential_(pp), geometry_(geom), extended_(ext){};

    private:
    // Components
    double total_, pairPotential_, geometry_, extended_;

    public:
    double total() const { return total_; };
    double pairPotential() const { return pairPotential_; };
    double geometry() const { return geometry_; };
    double extended() const { return extended_; };
    double totalUnbound() const { return pairPotential_ + extended_; }
};

// Standard Energy Kernel, inheriting GeometryKernel
class EnergyKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    EnergyKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                 std::optional<double> energyCutoff = {});

    public:
    ~EnergyKernel() = default;

    /*
     * Base Routines
     */
    private:
    // Return PairPotential energy between atoms
    virtual double pairPotentialEnergy(const Atom &i, const Atom &j, double r) const;
    // Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
    virtual double pairPotentialEnergy(const Atom &i, const Atom &j, double r, double elecScale, double vdwScale) const;

    /*
     * PairPotential Terms
     */
    private:
    // Return PairPotential energy of atoms in the supplied cell
    double cellEnergy(const Cell &cell, bool includeIntraMolecular) const;
    // Return PairPotential energy between two cells
    double cellToCellEnergy(const Cell &cell, const Cell &otherCell, bool applyMim, bool includeIntraMolecular) const;
    // Return PairPotential energy of atom with world
    double pairPotentialEnergy(const Atom &i) const;
    // Return PairPotential energy of Molecule with world
    double pairPotentialEnergy(const Molecule &mol, bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const;

    /*
     * Extended Terms
     */
    private:
    // Return energy of supplied atom from ad hoc extended terms
    virtual double extendedEnergy(const Atom &i) const;
    // Return energy of supplied molecule from ad hoc extended terms
    virtual double extendedEnergy(const Molecule &mol) const;

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
    double totalPairPotentialEnergy(bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const;
    // Return total interatomic PairPotential energy from summation of molecules
    double totalMoleculePairPotentialEnergy(bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const;
    // Return total energy of supplied atom with the world
    EnergyResult totalEnergy(const Atom &i) const;
    // Return total energy of supplied molecule with the world
    EnergyResult totalEnergy(const Molecule &mol, ProcessPool::DivisionStrategy strategy,
                             Flags<EnergyCalculationFlags> flags = {}) const;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/processPool.h"
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

// PairPotential Energy Value
class PairPotentialEnergyValue
{
    public:
    PairPotentialEnergyValue(double inter = 0.0, double intra = 0.0);
    PairPotentialEnergyValue operator+(const PairPotentialEnergyValue &value) const;
    PairPotentialEnergyValue operator-(const PairPotentialEnergyValue &value) const;
    PairPotentialEnergyValue &operator+=(const PairPotentialEnergyValue &value);
    PairPotentialEnergyValue &operator*=(const double scale);

    private:
    // Energy values
    double interMolecular_{0.0}, intraMolecular_{0.0};

    public:
    // Increment Energies
    void addInterMolecular(double e);
    void addIntraMolecular(double e);
    // Return Energies
    double interMolecular() const;
    double intraMolecular() const;
    double total() const;
};

// Energy Result
class EnergyResult
{
    public:
    EnergyResult(PairPotentialEnergyValue pp = {}, double geom = 0.0, double ext = 0.0)
        : total_(pp.total() + geom + ext), pairPotential_(pp), geometry_(geom), extended_(ext){};

    private:
    // Components
    PairPotentialEnergyValue pairPotential_;
    double total_, geometry_, extended_;

    public:
    double total() const { return total_; };
    PairPotentialEnergyValue pairPotential() const { return pairPotential_; };
    double geometry() const { return geometry_; };
    double extended() const { return extended_; };
    double totalUnbound() const { return pairPotential_.total() + extended_; }
};

// Standard Energy Kernel, inheriting GeometryKernel
class EnergyKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    friend class ExternalPotentialsEnergyKernel;
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
    PairPotentialEnergyValue cellEnergy(const Cell &cell, bool includeIntraMolecular) const;
    // Return PairPotential energy between two cells
    PairPotentialEnergyValue cellToCellEnergy(const Cell &cell, const Cell &otherCell, bool applyMim,
                                              bool includeIntraMolecular) const;
    // Return PairPotential energy of atom with world
    double pairPotentialEnergy(const Atom &i) const;
    // Return PairPotential energy of Molecule with world
    PairPotentialEnergyValue pairPotentialEnergy(const Molecule &mol, bool includeIntraMolecular) const;

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
    PairPotentialEnergyValue totalPairPotentialEnergy(bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const;
    // Return total interatomic PairPotential energy from summation of molecules
    PairPotentialEnergyValue totalMoleculePairPotentialEnergy(bool includeIntraMolecular) const;
    // Return total energy of supplied atom with the world
    EnergyResult totalEnergy(const Atom &i) const;
    // Return total energy of supplied molecule with the world
    EnergyResult totalEnergy(const Molecule &mol, Flags<EnergyCalculationFlags> flags = {}) const;
};

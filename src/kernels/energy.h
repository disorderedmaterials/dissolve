// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "kernels/geometry.h"
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

    public:
    // Return PairPotential energy of atom with world
    double pairPotentialEnergy(const Atom &i) const;
    // Return PairPotential energy of Molecule with world
    double pairPotentialEnergy(const Molecule &mol, bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const;
    // Return total interatomic PairPotential energy of the world
    double totalPairPotentialEnergy(bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const;
};

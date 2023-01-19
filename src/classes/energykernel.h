// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/kernelflags.h"
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

// Energy Kernel
class EnergyKernel
{
    public:
    EnergyKernel(const ProcessPool &procPool, const Box *box, const CellArray &cells, const PotentialMap &potentialMap,
                 std::optional<double> energyCutoff = std::nullopt);
    ~EnergyKernel() = default;
    // Clear all data
    void clear();

    /*
     * Source Data
     */
    protected:
    // Source Box (from Configuration)
    const Box *box_;
    // Source CellArray (from Configuration)
    const CellArray &cellArray_;
    // Potential map to use
    const PotentialMap &potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;

    /*
     * Base Routines
     */
    private:
    // Return PairPotential energy between atoms
    virtual double pairPotentialEnergy(const Atom &i, const Atom &j, double r);
    // Return PairPotential energy between atoms provided
    double energyWithoutMim(const Atom &i, const Atom &j);
    // Return PairPotential energy between atoms provided
    double energyWithMim(const Atom &i, const Atom &j);

    /*
     * PairPotential Terms
     */
    private:
    // Return PairPotential energy between atoms
    double energy(const Atom &i, const Atom &j, bool applyMim, bool excludeIgeJ);
    // Return PairPotential energy of atoms in the supplied cell with all other cells
    double energy(const Cell &cell, bool includeIntraMolecular);
    // Return PairPotential energy between two cells
    double energy(const Cell &cell, const Cell &otherCell, bool applyMim, bool includeIntraMolecular);

    public:
    // Return PairPotential energy of atom with world
    double energy(const Atom &i);
    // Return molecular correction energy related to intramolecular terms involving supplied atom
    double correct(const Atom &i);
    // Return PairPotential energy of Molecule with world
    double energy(const Molecule &mol, bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy);
    // Return total interatomic PairPotential energy of the system
    double energy(const CellArray &cellArray, bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy);

    /*
     * Intramolecular Terms
     */
    public:
    // Return SpeciesBond energy at Atoms specified
    double energy(const SpeciesBond &b, const Atom &i, const Atom &j);
    // Return SpeciesAngle energy at Atoms specified
    double energy(const SpeciesAngle &a, const Atom &i, const Atom &j, const Atom &k);
    // Return SpeciesTorsion energy at Atoms specified
    double energy(const SpeciesTorsion &t, const Atom &i, const Atom &j, const Atom &k, const Atom &l);
    // Return SpeciesImproper energy at Atoms specified
    double energy(const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k, const Atom &l);
    // Return intramolecular energy for the supplied Atom
    double intramolecularEnergy(const Molecule &mol, const Atom &i);
    // Return intramolecular energy for the supplied Molecule
    double intramolecularEnergy(const Molecule &mol);

    /*
     * Parallel Comms
     */
    private:
    // Process pool over which this kernel operates
    const ProcessPool &processPool_;
};

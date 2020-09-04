/*
    *** EnergyKernel
    *** src/classes/energykernel.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/processpool.h"
#include "classes/kernelflags.h"
#include "templates/orderedpointerlist.h"
#include <memory>

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
    EnergyKernel(ProcessPool &procPool, Configuration *config, const PotentialMap &potentialMap, double energyCutoff = -1.0);
    ~EnergyKernel();
    // Clear all data
    void clear();

    /*
     * Source Data
     */
    protected:
    // Source Configuration
    const Configuration *configuration_;
    // Source Box (from Configuration)
    const Box *box_;
    // Source CellArray (from Configuration)
    const CellArray &cells_;
    // Potential map to use
    const PotentialMap &potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;

    /*
     * Internal Routines
     */
    private:
    // Return PairPotential energy between atoms provided as pointers, at the distance specified
    virtual double pairPotentialEnergy(const Atom *i, const Atom *j, double r);
    // Return PairPotential energy between atoms provided as pointers (no minimum image calculation)
    double energyWithoutMim(const Atom *i, const Atom *j);
    // Return PairPotential energy between atoms provided as pointers (minimum image calculation)
    double energyWithMim(const Atom *i, const Atom *j);

    /*
     * PairPotential Terms
     */
    public:
    // Return PairPotential energy between atoms provided (as pointers)
    double energy(const Atom *i, const Atom *j, bool applyMim, bool excludeIgeJ);
    // Return PairPotential energy between two cells
    double energy(Cell *cell, Cell *otherCell, bool applyMim, bool excludeIgeJ, bool interMolecular,
                  ProcessPool::DivisionStrategy strategy, bool performSum);
    // Return PairPotential energy between Cell and its neighbours
    double energy(Cell *cell, bool excludeIgeJ, bool interMolecular, ProcessPool::DivisionStrategy strategy, bool performSum);
    // Return PairPotential energy between Atom and Cell
    double energy(const Atom *i, const Cell *cell, int flags, ProcessPool::DivisionStrategy strategy, bool performSum);
    // Return PairPotential energy of atom with world
    double energy(const Atom *i, ProcessPool::DivisionStrategy strategy, bool performSum);
    // Return PairPotential energy of Molecule with world
    double energy(std::shared_ptr<const Molecule> mol, ProcessPool::DivisionStrategy strategy, bool performSum);
    // Return molecular correction energy related to intramolecular terms involving supplied atom
    double correct(const Atom *i);
    // Return total interatomic PairPotential energy of the system
    double energy(const CellArray &cellArray, bool interMolecular, ProcessPool::DivisionStrategy strategy, bool performSum);

    /*
     * Intramolecular Terms
     */
    public:
    // Return SpeciesBond energy at Atoms specified
    double energy(const SpeciesBond &b, const Atom *i, const Atom *j);
    // Return SpeciesBond energy
    static double energy(const SpeciesBond &b);
    // Return SpeciesAngle energy at Atoms specified
    double energy(const SpeciesAngle &a, const Atom *i, const Atom *j, const Atom *k);
    // Return SpeciesAngle energy
    static double energy(const SpeciesAngle &a);
    // Return SpeciesTorsion energy at Atoms specified
    double energy(const SpeciesTorsion &t, const Atom *i, const Atom *j, const Atom *k, const Atom *l);
    // Return SpeciesTorsion energy
    static double energy(const SpeciesTorsion &t);
    // Return SpeciesImproper energy
    double energy(const SpeciesImproper &imp, const Atom *i, const Atom *j, const Atom *k, const Atom *l);
    // Return intramolecular energy for the supplied Atom
    double intramolecularEnergy(std::shared_ptr<const Molecule> mol, const Atom *i);
    // Return intramolecular energy for the supplied Molecule
    double intramolecularEnergy(std::shared_ptr<const Molecule> mol);

    /*
     * Parallel Comms
     */
    private:
    // Process pool over which this kernel operates
    ProcessPool &processPool_;
};

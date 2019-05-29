/*
	*** EnergyKernel
	*** src/classes/energykernel.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_ENERGYKERNEL_H
#define DISSOLVE_ENERGYKERNEL_H

#include "classes/kernelflags.h"
#include "templates/orderedpointerlist.h"
#include "base/processpool.h"

// Forward Declarations
class Atom;
class Grain;
class Cell;
class CellArray;
class Box;
class Configuration;
class PotentialMap;
class Molecule;
class Bond;
class Angle;
class Torsion;

// Energy Kernel
class EnergyKernel
{
	public:
	// Constructor
	EnergyKernel(ProcessPool& procPool, Configuration* config, const PotentialMap& potentialMap, double energyCutoff = -1.0);
	// Destructor
	~EnergyKernel();
	// Clear all data
	void clear();


	/*
	 * Source Data
	 */
	protected:
	// Source Configuration
	const Configuration* configuration_;
	// Source Box (from Configuration)
	const Box* box_;
	// Source CellArray (from Configuration)
	const CellArray& cells_;
	// Potential map to use
	const PotentialMap& potentialMap_;
	// Squared cutoff distance to use in calculation
	double cutoffDistanceSquared_;


	/*
	 * Internal Routines
	 */
	private:
	// Return PairPotential energy between atoms provided as pointers, at the distance specified
	virtual double pairPotentialEnergy(const Atom* i, const Atom* j, double r);
	// Return PairPotential energy between atoms provided as pointers (no minimum image calculation)
	double energyWithoutMim(const Atom* i, const Atom* j);
	// Return PairPotential energy between atom (pointer) and grain provided (no minimum image calculation)
	double energyWithoutMim(const Atom* i, const Grain* grain, bool excludeIgeJ = false);
	// Return PairPotential energy between grains provided (no minimum image calculation)
	double energyWithoutMim(const Grain* grainI, const Grain* grainJ);
	// Return PairPotential energy between atoms provided as pointers (minimum image calculation)
	double energyWithMim(const Atom* i, const Atom* j);
	// Return PairPotential energy between atom (pointer) and Grain provided (minimum image calculation)
	double energyWithMim(const Atom* i, const Grain* grain, bool excludeIgeJ = false);
	// Return PairPotential energy between Grains provided (minimum image calculation)
	double energyWithMim(const Grain* grainI, const Grain* grainJ);


	/*
	 * PairPotential Terms
	 */
	public:
	// Return PairPotential energy between atoms provided (as pointers)
	double energy(const Atom* i, const Atom* j, bool applyMim, bool excludeIgeJ);
	// Return PairPotential energy between two cells
	double energy(Cell* cell, Cell* otherCell, bool applyMim, bool excludeIgeJ, bool interMolecular, ProcessPool::DivisionStrategy strategy, bool performSum);
	// Return PairPotential energy between Cell and its neighbours
	double energy(Cell* cell, bool excludeIgeJ, bool interMolecular, ProcessPool::DivisionStrategy strategy, bool performSum);
	// Return PairPotential energy between Atom and Cell
	double energy(const Atom* i, Cell* cell, int flags, ProcessPool::DivisionStrategy strategy, bool performSum);
	// Return PairPotential energy of atom with world
	double energy(const Atom* i, ProcessPool::DivisionStrategy strategy, bool performSum);
	// Return PairPotential energy of grain with world
	double energy(const Grain* grain, bool excludeIgtJ, ProcessPool::DivisionStrategy strategy, bool performSum);
	// Return PairPotential energy of Molecule with world
	double energy(const Molecule* mol, ProcessPool::DivisionStrategy strategy, bool performSum);
	// Return molecular correction energy related to intramolecular terms involving supplied atom
	double correct(const Atom* i);
	// Return total interatomic PairPotential energy of the system
	double energy(const CellArray& cellArray, bool interMolecular, ProcessPool::DivisionStrategy strategy, bool performSum);


	/*
	 * Intramolecular Terms
	 */
	public:
	// Return Bond energy
	double energy(const Bond* b);
	// Return Angle energy
	double energy(const Angle* a);
	// Return Torsion energy
	double energy(const Torsion* t);
	// Return intramolecular energy for the supplied Atom
	double intraEnergy(const Atom* i);
	// Return intramolecular energy for the supplied Molecule
	double intraEnergy(const Molecule* mol);


	/*
	 * Parallel Comms
	 */
	private:
	// Process pool over which this kernel operates
	ProcessPool& processPool_;
};

#endif

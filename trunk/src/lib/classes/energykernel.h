/*
	*** EnergyKernel
	*** src/lib/classes/energykernel.h
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_ENERGYKERNEL_H
#define DUQ_ENERGYKERNEL_H

#include "classes/atom.h"
#include "classes/grain.h"
#include "templates/orderedlist.h"
#include "base/comms.h"

// Forward Declarations
class Cell;
class Box;
class Configuration;
class PotentialMap;
class Molecule;
class SpeciesBond;
class SpeciesAngle;

/*!
 * \brief Energy Kernel
 * \details An EnergyKernel provides PairPotential energy calculation routines between Atoms, Grains, and Cells, and any combination thereof.
 */
class EnergyKernel
{
	public:
	// Constructor
	EnergyKernel(const Configuration& config, const PotentialMap& potentialMap, double cutoffDistanceSquared = -1.0);
	// Destructor
	~EnergyKernel();
	// Clear all data
	void clear();
	// Flags
	enum Flags
	{
		NoFlags = 0,
		ExcludeSelfFlag = 1,
		ExcludeGreaterThanEqualTo = 2,
		ExcludeIntraGreaterThan = 4,
		ApplyMinimumImage = 8
	};


	/*!
	 * \name Source Data
	 */
	///@{
	private:
	// Source Configuration
	const Configuration& configuration_;
	// Source Box (from Configuration)
	const Box* box_;
	// Potential map to use
	const PotentialMap& potentialMap_;
	// Squared cutoff distance to use in calculation
	double cutoffDistanceSquared_;
	///@}


	/*!
	 * \name Internal Routines
	 */
	///@{
	private:
	// Return PairPotential energy between atoms provided as pointers (no minimum image calculation)
	double energyWithoutMim(const Atom* i, const Atom* j);
	// Return PairPotential energy between atoms provided as info and reference (minimum image calculation)
	double energyWithoutMim(const int typeI, const Vec3<double>& rI, const Atom* j);
	// Return PairPotential energy between atoms provided as references (no minimum image calculation)
	double energyWithoutMim(const Atom& i, const Atom& j);
	// Return PairPotential energy between atoms provided as reference/pointer (no minimum image calculation)
	double energyWithoutMim(const Atom& i, const Atom* j);
	// Return PairPotential energy between atom (pointer) and grain provided (no minimum image calculation)
	double energyWithoutMim(const Atom* i, const Grain* grain, bool excludeIgeJ = false);
	// Return PairPotential energy between atom (reference) and grain provided (no minimum image calculation)
	double energyWithoutMim(const Atom& i, const Grain* grain, bool excludeIgeJ = false);
	// Return PairPotential energy between grains provided (no minimum image calculation)
	double energyWithoutMim(const Grain* grainI, const Grain* grainJ);
	// Return PairPotential energy between atoms provided as pointers (minimum image calculation)
	double energyWithMim(const Atom* i, const Atom* j);
	// Return PairPotential energy between atoms provided as info and reference (minimum image calculation)
	double energyWithMim(const int typeI, const Vec3<double>& rI, const Atom* j);
	// Return PairPotential energy between atoms provided as references (minimum image calculation)
	double energyWithMim(const Atom& i, const Atom& j);
	// Return PairPotential energy between atoms provided as reference/pointer (minimum image calculation)
	double energyWithMim(const Atom& i, const Atom* j);
	// Return PairPotential energy between atom (pointer) and Grain provided (minimum image calculation)
	double energyWithMim(const Atom* i, const Grain* grain, bool excludeIgeJ = false);
	// Return PairPotential energy between atom (reference) and grain provided (minimum image calculation)
	double energyWithMim(const Atom& i, const Grain* grain, bool excludeIgeJ = false);
	// Return PairPotential energy between Grains provided (minimum image calculation)
	double energyWithMim(const Grain* grainI, const Grain* grainJ);
	///@}


	/*!
	 * \brief PairPotential Terms
	 */
	///@{
	public:
	// Return PairPotential energy between atoms provided (as pointers)
	double energy(const Atom* i, const Atom* j, bool applyMim, bool excludeIgeJ = false);
	// Return PairPotential energy between two cells
	double energy(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ = false, DUQComm::CommGroup group = DUQComm::Solo);
	// Return PairPotential energy between cell and atomic neighbours
	double energy(Cell* centralCell, bool excludeIgeJ = false, DUQComm::CommGroup group = DUQComm::Solo);
	// Return PairPotential energy between atom and cell
	double energy(const Atom* i, OrderedList<Atom>& neighbours, int flags = EnergyKernel::NoFlags, DUQComm::CommGroup group = DUQComm::Solo);
	// Return PairPotential energy of atom with world
	double energy(const Atom* i, DUQComm::CommGroup group = DUQComm::Solo);
	// Return PairPotential energy between Grain and list of neighbouring cells
	double energy(const Grain* grain, OrderedList<Atom>& neighbours, bool applyMim, bool excludeIgeJ = false, DUQComm::CommGroup group = DUQComm::Solo);
	// Return PairPotential energy of grain with world
	double energy(const Grain* grain, bool excludeIgtJ, DUQComm::CommGroup group = DUQComm::Solo);
	// Return molecular correction energy related to intramolecular terms involving supplied atom
	double correct(const Atom* i);
	// Return molecular correction energy related to intramolecular terms involving atoms in supplied grain
	double correct(const Grain* i);
	///@}


	/*!
	 * \brief Intramolecular Terms
	 */
	///@{
	public:
	// Return Bond energy
	double energy(const Molecule* mol, const SpeciesBond* b);
	// Return Angle energy
	double energy(const Molecule* mol, const SpeciesAngle* a);
	// Return full intramolecular energy
	double fullIntraEnergy(const Grain* grain, double termFactor = 1.0);
	///@}


	/*!
	 * \brief Molecule Terms
	 */
	///@{
	public:
	// Return total Molecule energy
	double energy(Molecule* mol, DUQComm::CommGroup group = DUQComm::Solo, bool halfPP = false, double ppFactorIntra = 1.0, double termFactor = 1.0);
	///@}
};

#endif

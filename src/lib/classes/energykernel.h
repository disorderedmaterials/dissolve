/*
	*** EnergyKernel
	*** src/lib/classes/energykernel.h
	Copyright T. Youngs 2012-2013

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
#include "base/comms.h"

// Forward Declarations
class Cell;
class Box;
class PotentialMap;
class Molecule;

/*!
 * \brief Energy Kernel
 * \details An EnergyKernel provides PairPotential energy calculation routines between Atoms, Grains, and Cells, and any combination thereof.
 */
class EnergyKernel
{
	public:
	// Constructor
	EnergyKernel(const Box* box, const PotentialMap& potentialMap);
	// Destructor
	~EnergyKernel();
	// Clear all data
	void clear();


	/*!
	 * \name Source Data
	 */
	///@{
	protected:
	// Source Box
	const Box* box_;
	// Potential map to use
	const PotentialMap& potentialMap_;
	///@}


	/*!
	 * \name Internal Routines
	 */
	///@{
	private:
	// Return PairPotential energy between Atoms provided (no minimum image calculation)
	double energyWithoutMim(const Atom* i, const Atom* j);
	// Return PairPotential energy between Atom and Grain provided (no minimum image calculation)
	double energyWithoutMim(const Atom* i, const Grain* grain, bool excludeIgtJ = FALSE);
	// Return PairPotential energy between Grains provided (no minimum image calculation)
	double energyWithoutMim(const Grain* grainI, const Grain* grainJ);
	// Return PairPotential energy between Atoms provided (minimum image calculation)
	double energyWithMim(const Atom* i, const Atom* j);
	// Return PairPotential energy between Atom and Grain provided (minimum image calculation)
	double energyWithMim(const Atom* i, const Grain* grain, bool excludeIgtJ = FALSE);
	// Return PairPotential energy between Grains provided (minimum image calculation)
	double energyWithMim(const Grain* grainI, const Grain* grainJ);
	///@}


	/*!
	 * \brief PairPotential Terms
	 */
	///@{
	public:
	// Return PairPotential energy between Atoms provided
	double energy(const Atom* i, const Atom* j, bool applyMim, bool excludeIgtJ = FALSE);
	// Return PairPotential energy between Atom and Grain provided
	double energy(const Atom* i, const Grain* grain, bool applyMim, bool excludeIgtJ = FALSE);
	// Return PairPotential energy between Grains provided
	double energy(const Grain* grainI, const Grain* grainJ, double cutoffSq, bool applyMim, bool excludeIgtJ = FALSE);
	// Return PairPotential energy between Atom and Cell contents
	double energy(const Atom* i, Cell* cell, bool applyMim, bool excludeIgtJ = FALSE);
	// Return PairPotential energy between Grain and Cell contents
	double energy(const Grain* grain, Cell* cell, double cutoffSq, bool applyMim, bool excludeIgtJ = FALSE, dUQComm::CommGroup group = dUQComm::Solo);
	// Return PairPotential energy between Atom and list of Cells
	double energy(const Atom* i, RefList<Cell,bool>& neighbours, bool excludeIgtJ = FALSE, dUQComm::CommGroup group = dUQComm::Solo);
	// Return PairPotential energy between Grain and list of Cells
	double energy(const Grain* grain, RefList<Cell,bool>& neighbours, double cutoffSq, bool excludeIgtJ = FALSE, dUQComm::CommGroup group = dUQComm::Solo);
	///@}


	/*!
	 * \brief Intramolecular Terms
	 */
	///@{
	public:
	// Return Bond energy
	double energy(const Bond* b);
	// Return Angle energy
	double energy(const Angle* a);
	// Return full intramolecular energy
	double fullIntraEnergy(const Grain* grain, double termFactor = 1.0);
	///@}


	/*!
	 * \brief Molecule Terms
	 */
	///@{
	public:
	// Return total Molecule energy
	double energy(Molecule* mol, double cutoffSq, dUQComm::CommGroup group = dUQComm::Solo, bool halfPP = FALSE, double ppFactorIntra = 1.0, double termFactor = 1.0);
	///@}
};

#endif

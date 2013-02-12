/*
	*** PotentialMap Definition
	*** src/classes/potentialmap.h
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

#ifndef DUQ_POTENTIALMAP_H
#define DUQ_POTENTIALMAP_H

#include "classes/atomtypeindex.h"
#include "base/constants.h"

// Forward Declarations
class PairPotential;
class Atom;
class Molecule;

/*!
 * \brief PotentialMap Definition
 * \details TODO
 */
class PotentialMap
{
	public:
	// Constructor
	PotentialMap();
	// Destructor
	~PotentialMap();
	// Clear all data
	void clear();


	/*!
	 * \name Source Parameters
	 */
	///@{
	private:
	// Size of types_ array
	int nAtoms_;
	// Atom->AtomType index
	int* typeMap_;
	// Number of unique types forming potentialMatrix_
	int nTypes_;
	// PairPotential matrix
	PairPotential*** potentialMatrix_;
	// PairPotential range
	double range_;
	// PairPotential range squared
	double rangeSquared_;

	public:
	// Initialise maps
	bool initialise(const AtomTypeIndex& typeIndex, const List<PairPotential>& pairPotentials, int nAtoms, Molecule* firstMolecule, double pairPotentialRange);
	// Return type index of Atom index supplied
	int type(int index);
	// Return PairPotential range
	double range() const;
	// Return PairPotential range squared
	double rangeSquared() const;
	///@}


	/*!
	 * \name Energy / Force
	 */
	///@{
	public:
	// Return energy between Atom types at squared distance specified
	double energy(int typeI, int typeJ, double distanceSquared) const;
	// Return energy between Atoms at squared distance specified
	double energy(const Atom* i, const Atom* j, double distanceSquared) const;
	// Return force between Atom types at squared distance specified
	double force(int typeI, int typeJ, double distanceSquared) const;
	// Return force between Atoms at squared distance specified
	double force(const Atom* i, const Atom* j, double distanceSquared) const;
	///@}
};

#endif

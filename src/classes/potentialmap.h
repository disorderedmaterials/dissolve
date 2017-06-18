/*
	*** PotentialMap Definition
	*** src/classes/potentialmap.h
	Copyright T. Youngs 2012-2017

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

#include "classes/atomtypelist.h"
#include "base/constants.h"
#include "templates/array2d.h"

// Forward Declarations
class PairPotential;
class Atom;
class Molecule;

// PotentialMap Definition
class PotentialMap
{
	public:
	// Constructor
	PotentialMap();
	// Destructor
	~PotentialMap();
	// Clear all data
	void clear();


	/*
	 * Source Parameters
	 */
	private:
	// Number of unique types forming potentialMatrix_
	int nTypes_;
	// PairPotential matrix
	Array2D<PairPotential*> potentialMatrix_;
	// PairPotential range
	double range_;
	// PairPotential range squared
	double rangeSquared_;

	public:
	// Initialise maps
	bool initialise(const List<AtomType>& masterAtomTypes, const List<PairPotential>& pairPotentials, double pairPotentialRange);
	// Return PairPotential range
	double range() const;
	// Return PairPotential range squared
	double rangeSquared() const;


	/*
	 * Energy / Force
	 */
	public:
	// Return energy between Atom types at squared distance specified
	double energy(int typeI, int typeJ, double distanceSquared) const;
	// Return force between Atom types at squared distance specified
	double force(int typeI, int typeJ, double distanceSquared) const;
};

#endif

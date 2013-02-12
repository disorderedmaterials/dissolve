/*
	*** ChangeStore
	*** src/lib/classes/changestore.h
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

#ifndef DUQ_CHANGESTORE_H
#define DUQ_CHANGESTORE_H

#include "templates/vector3.h"
#include "templates/pair.h"
#include "templates/array.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Grain;
class Molecule;

/*!
 * \brief ChangeStore
 */
class ChangeStore
{
	public:
	// Constructor
	ChangeStore();
	// Destructor
	~ChangeStore();


	/*!
	 * \name Watch Targets
	 */
	///@{
	private:
	// List of target Atoms (and modification data)
	RefList<Atom, Pair< bool,Vec3<double> > > targetAtoms_;

	public:
	// Add Grain to watch
	void add(Grain* grain);
	// Add Molecule (Grains) to watch
	void add(Molecule* mol);
	///@}


	/*!
	 * \name Change Data
	 */
	///@{
	private:
	// List of local changes
	List< Pair< int,Vec3<double> > > changes_;
	// Coordinate broadcast arrays
	Array<double> x_, y_, z_;
	// Index broadcast array
	Array<int> indices_;

	public:
	// Reset ChangeStore, forgetting all changes
	void reset();
	// Update all Atom positions
	void updateAll();
	// Update Atom positions using list indices
	void updateAtomsLocal(int nAtoms, int* indices);
	// Update Atom positions using relative Atom indices
	void updateAtomsRelative(int nAtoms, Atom** atoms, int rootIndex);
	// Revert to stored Atom positions
	void revert();
	// Save Atom changes for broadcast, and reset arrays for new data
	void storeAndReset();
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Distribute change data to all processes
	bool distribute(int nAtoms, Atom* atoms);
	///@}
};

#endif

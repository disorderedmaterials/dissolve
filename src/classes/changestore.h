/*
	*** ChangeStore
	*** src/classes/changestore.h
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

#ifndef DUQ_CHANGESTORE_H
#define DUQ_CHANGESTORE_H

#include "templates/vector3.h"
#include "templates/pair.h"
#include "templates/array.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Cell;
class Grain;
class Molecule;
class Configuration;

/*
 * \brief Change Data
 */
class ChangeData
{
	public:
	// Constructor
	ChangeData();
	// Destructor
	~ChangeData();
	// List pointers
	ChangeData* next, *prev;


	/*
	 * Target Data
	 */
	private:
	// Atom
	Atom* atom_;
	// Flag indicating that atom has moved
	bool moved_;
	// Current coordinates of Atom (either original or newly-accepted)
	Vec3<double> r_;

	public:
	// Set target atom
	void setAtom(Atom* i);
	// Return index of stored atom
	int atomIndex();
	// Update stored position, and flag as moved
	void updatePosition();
	// Revert atom to stored position
	void revertPosition();
	// Return whether atom has moved
	bool hasMoved();
};

/*
 * \brief ChangeStore
 */
class ChangeStore
{
	public:
	// Constructor
	ChangeStore();
	// Destructor
	~ChangeStore();


	/*
	 * Watch Targets
	 */
	private:
	// List of target atoms (and modification data)
	List<ChangeData> targetAtoms_;

	public:
	// Add atom to watch
	void add(Atom* i);
	// Add grain to watch
	void add(Grain* grain);
	// Add molecule to watch
	void add(Molecule* mol);
	// Add cell to watch
	void add(Cell* cell);


	/*
	 * Change Data
	 */
	private:
	// List of local changes
	List<ChangeData> changes_;
	// Coordinate broadcast arrays
	Array<double> x_, y_, z_;
	// Index broadcast array
	Array<int> indices_;

	public:
	// Reset ChangeStore, forgetting all changes
	void reset();
	// Update all Atom positions
	void updateAll();
	// Update single atom position
	void updateAtom(int id);
	// Update Atom positions using list indices
	void updateAtomsLocal(int nAtoms, int* indices);
	// Revert all atoms to stored positions
	void revertAll();
	// Revert specified index to stored position
	void revert(int id);
	// Save Atom changes for broadcast, and reset arrays for new data
	void storeAndReset();


	/*
	 * Parallel Comms
	 */
	public:
	// Distribute and apply change data to all processes
	bool distributeAndApply(Configuration& cfg);
};

#endif

/*
	*** ChangeStore
	*** src/lib/classes/changestore.cpp
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

#include "classes/changestore.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/grain.h"
#include "classes/molecule.h"
#include "templates/orderedlist.h"

/*!
 * \brief Constructor
 * \details Constructor for ChangeStore. 
 */
ChangeStore::ChangeStore()
{
}

/*!
 * \brief Destructor
 * \details Destructor for ChangeStore. 
 */
ChangeStore::~ChangeStore()
{
}

/*
// Watch Targets
*/

/*!
 * \brief Add atom to watch
 */
void ChangeStore::add(Atom* i)
{
	ChangeData* item = targetAtoms_.add();
	item->setAtom(i);
}

/*!
 * \brief Add Grain to watch
 */
void ChangeStore::add(Grain* grain)
{
	for (int n=0; n<grain->nAtoms(); ++n) add(grain->atom(n));
}

/*!
 * \brief Add Molecule to watch
 */
void ChangeStore::add(Molecule* mol)
{
	for (int n=0; n<mol->nAtoms(); ++n) add(mol->atom(n));
}

/*!
 * \brief Add Cell to watch
 * \details Add all the atoms in the specified cell to the watch list
 */
void ChangeStore::add(Cell* cell)
{
	for (OrderedListItem<Atom>* item = cell->atoms().first(); item != NULL; item = item->next) add(item->object());
}

/*
// Change Array
*/

/*!
 * \brief Reset ChangeStore
 * \details Reset the ChangeStore, forgetting all targets and change data
 */
void ChangeStore::reset()
{
	targetAtoms_.clear();
	changes_.clear();
}

/*!
 * \brief Update all Atom positions
 */
void ChangeStore::updateAll()
{
	for (ChangeData* item = targetAtoms_.first(); item != NULL; item = item->next) item->updatePosition();
}

/*!
 * \brief Update Atom positions using list indices
 */
void ChangeStore::updateAtomsLocal(int nAtoms, int* indices)
{
	for (int n=0; n<nAtoms; ++n)
	{
#ifdef CHECKS
		if ((indices[n] < 0) || (indices[n] >= targetAtoms_.nItems()))
		{
			msg.print("OUT_OF_RANGE - Supplied indices_[n] (%i) is out of range in ChangeStore::updateAtomsLocal() (nTargetAtoms = %i)\n", n, indices_[n], targetAtoms_.nItems());
			continue;
		}
#endif
		ChangeData* item = targetAtoms_[indices[n]];
		item->updatePosition();
	}
}

/*!
 * \brief Update single atom position
 */
void ChangeStore::updateAtom(int id)
{
#ifdef CHECKS
	if ((id < 0) || (id >= targetAtoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Specified index %i is out of range in ChangeStore::updateAtom() (nTargetAtoms = %i)\n", id, targetAtoms_.nItems());
		return;
	}
#endif
	ChangeData* item = targetAtoms_[id];
	item->updatePosition();
}

/*!
 * \brief Revert all atoms to their previous positions
 */
void ChangeStore::revertAll()
{
// 	printf("In Revert...\n");
	for (ChangeData* item = targetAtoms_.first(); item != NULL; item = item->next) item->revertPosition();
// 	printf("Done Revert.\n");
}

/*!
 * \brief Revert specified index to stored position
 */
void ChangeStore::revert(int id)
{
#ifdef CHECKS
	if ((id < 0) || (id >= targetAtoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index of Atom (%i) is out of range in ChangeStore::revert() (nAtoms = %i).\n", id, targetAtoms_.nItems());
		return;
	}
#endif
	ChangeData* item = targetAtoms_[id];
	item->revertPosition();
}

/*!
 * \brief Save Atom changes for broadcast, and reset arrays for new data
 */
void ChangeStore::storeAndReset()
{
	ChangeData* item = targetAtoms_.first();
	ChangeData* nextItem;
	while (item != NULL)
	{
		// Grab pointer to next item
		nextItem = item->next;

		// Has the position of this Atom been changed (i.e. updated)?
		if (item->hasMoved())
		{
			targetAtoms_.cut(item);
			changes_.own(item);
		}

		item = nextItem;
	}

	// Clear target Atom data
	targetAtoms_.clear();
}

/*!
 * \brief Distribute and apply changes
 */
bool ChangeStore::distribute(Configuration& cfg)
{
#ifdef PARALLEL
	// First, get total number of changes across all processes
	int nTotalChanges = changes_.nItems();
	if (!Comm.allSum(&nTotalChanges, 1)) return false;

// 	msg.print("We think there are %i changes in total to distribute.\n", nTotalChanges);

	// All processes now resize their arrays so they are large enough to hold the total number of changes
	if (nTotalChanges == 0) return true;
	x_.createEmpty(nTotalChanges);
	y_.createEmpty(nTotalChanges);
	z_.createEmpty(nTotalChanges);
	indices_.createEmpty(nTotalChanges);
	
	// Copy local change data into arrays
	for (int n=0; n<changes_.nItems(); ++n)
	{
		indices_[n] = changes_[n]->a;
		x_[n] = changes_[n]->b.x;
		y_[n] = changes_[n]->b.y;
		z_[n] = changes_[n]->b.z;
	}

	// Now, assemble full array of the change data on the master...
	if (!Comm.assemble(indices_, changes_.nItems(), indices_, nTotalChanges)) return false;
	if (!Comm.assemble(x_, changes_.nItems(), x_, nTotalChanges)) return false;
	if (!Comm.assemble(y_, changes_.nItems(), y_, nTotalChanges)) return false;
	if (!Comm.assemble(z_, changes_.nItems(), z_, nTotalChanges)) return false;
	
	// ... then broadcast it to the slaves
	if (!Comm.broadcast(indices_, nTotalChanges)) return false;
	if (!Comm.broadcast(x_, nTotalChanges)) return false;
	if (!Comm.broadcast(y_, nTotalChanges)) return false;
	if (!Comm.broadcast(z_, nTotalChanges)) return false;

	// Apply atom changes
	Atom** atoms = cfg.atomReferences();
	for (int n=0; n<nTotalChanges; ++n)
	{
#ifdef CHECKS
		if ((indices_[n] < 0) || (indices_[n] >= cfg.nAtoms()))
		{
			msg.print("OUT_OF_RANGE - Index of Atom change (%i) is out of range in ChangeStore::distribute() (nAtoms = %i).\n", indices_[n], cfg.nAtoms());
			continue;
		}
#endif
		// Set new coordinates and check cell position
		if (atoms[indices_[n]]->index() == Atom::UnusedAtom) continue;
		atoms[indices_[n]]->setCoordinates(x_[n], y_[n], z_[n]);
		cfg.updateAtomInCell(atoms[indices_[n]);
	}
#else

	// Apply atom changes
	for (ChangeData* data = changes_.first(); data != NULL; data = data->next)
	{
		// Set new coordinates and check cell position (Configuration::updateAtomInCell() will do all this)
		data->revertPosition();
		cfg.updateAtomInCell(data->atomIndex());
	}
#endif
	return true;
}

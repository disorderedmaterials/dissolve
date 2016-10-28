/*
	*** Grain Definition
	*** src/classes/graindefinition.cpp
	Copyright T. Youngs 2012-2016

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

#include "classes/graindefinition.h"
#include "base/ptable.h"
#include "base/processpool.h"

// Constructor
GrainDefinition::GrainDefinition() : ListItem<GrainDefinition>()
{
	parent_ = NULL;
}

// Destructor
GrainDefinition::~GrainDefinition()
{
}

/*
 * Basic Character
 */

// Set parent Species
void GrainDefinition::setParent(Species* parent)
{
	parent_ = parent;
}

// Return parent Species
Species *GrainDefinition::parent() const
{
	return parent_;
}

// Set name of GrainDefinition
void GrainDefinition::setName(const char* s)
{
	name_ = s;
}

// Return name of GrainDefinition
const char* GrainDefinition::name() const
{
	return name_.get();
}

/*
 * Atoms
 */

// Search current list for Atom
bool GrainDefinition::containsAtom(Atom* i) const
{
	return atoms_.contains(i);
}

// Add specified Atom to list
void GrainDefinition::addAtom(Atom* i)
{
	atoms_.add(i);
}

// Remove specified Atom from list
void GrainDefinition::removeAtom(Atom* i)
{
	atoms_.remove(i);
}

// Return number of Atoms in the list
int GrainDefinition::nAtoms() const
{
	return atoms_.nItems();
}

// Return first Atom reference in list
RefListItem<Atom,int>* GrainDefinition::atoms() const
{
	return atoms_.first();
}

// Return nth Atom reference in list
RefListItem<Atom,int>* GrainDefinition::atom(int n)
{
	return atoms_[n];
}

// Return empirical name of GrainDefinition from constituent Atoms
const char* GrainDefinition::nameFromAtoms()
{
	PeriodicTable::resetEmpiricalFormula();
	for (RefListItem<Atom,int>* ri = atoms_.first(); ri != NULL; ri = ri->next) PeriodicTable::addToEmpirical(ri->item->element());
	return PeriodicTable::empiricalFormula();
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool GrainDefinition::broadcast(const List<Atom>& atoms)
{
#ifdef PARALLEL
	int n, count, index;

	// Name
	if (!procPool.broadcast(name_)) return false;
	
	// Atoms
	count = atoms_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = atoms_.item(n)->item->index();
		if (!procPool.broadcast(&index, 1)) return false;
		if (Comm.slave()) addAtom(atoms.item(index));
	}
#endif
	return true;
}

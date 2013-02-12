/*
	*** Grain Definition
	*** src/lib/classes/graindefinition.cpp
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

#include "classes/graindefinition.h"
#include "base/ptable.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for GrainDefinition. 
 */
GrainDefinition::GrainDefinition() : ListItem<GrainDefinition>()
{
	parent_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for GrainDefinition. 
 */
GrainDefinition::~GrainDefinition()
{
}

/*
// Basic Character
*/

/*!
 * \brief Set parent Species
 */
void GrainDefinition::setParent(Species* parent)
{
	parent_ = parent;
}

/*!
 * \brief Return parent Species
 */
Species *GrainDefinition::parent() const
{
	return parent_;
}

/*!
 * \brief Set name of GrainDefinition
 */
void GrainDefinition::setName(const char* s)
{
	name_ = s;
}

/*!
 * \brief Return name of GrainDefinition
 */
const char *GrainDefinition::name() const
{
	return name_.get();
}

/*
// Atoms
*/

/*!
 * \brief Search current list for Atom
 */
bool GrainDefinition::containsAtom(Atom* i) const
{
	return atoms_.contains(i);
}

/*!
 * \brief Add specified Atom to list
 */
void GrainDefinition::addAtom(Atom* i)
{
	atoms_.add(i);
}

/*!
 * \brief Remove specified Atom from list
 */
void GrainDefinition::removeAtom(Atom* i)
{
	atoms_.remove(i);
}

/*!
 * \brief Return number of Atoms in the list
 */
int GrainDefinition::nAtoms() const
{
	return atoms_.nItems();
}

/*!
 * \brief Return first Atom reference in list
 */
RefListItem<Atom,int>* GrainDefinition::atoms() const
{
	return atoms_.first();
}

/*!
 * \brief Return nth Atom reference in list
 */
RefListItem<Atom,int>* GrainDefinition::atom(int n)
{
	return atoms_[n];
}

/*!
 * \brief Return empirical name of GrainDefinition from constituent Atoms
 * \details Creates and returns a name for the GrainDefinition which corresponds to the empirical formula of its Atoms
 */
const char* GrainDefinition::nameFromAtoms()
{
	PeriodicTable::resetEmpiricalFormula();
	for (RefListItem<Atom,int>* ri = atoms_.first(); ri != NULL; ri = ri->next) PeriodicTable::addToEmpirical(ri->item->element());
	return PeriodicTable::empiricalFormula();
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool GrainDefinition::broadcast(const List<Atom>& atoms)
{
#ifdef PARALLEL
	int n, count, index;

	// Name
	if (!Comm.broadcast(name_)) return FALSE;
	
	// Atoms
	count = atoms_.nItems();
	if (!Comm.broadcast(&count, 1)) return FALSE;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = atoms_.item(n)->item->index();
		if (!Comm.broadcast(&index, 1)) return FALSE;
		if (Comm.slave()) addAtom(atoms.item(index));
	}
#endif
	return TRUE;
}

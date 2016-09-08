/*
	*** Species Grain
	*** src/classes/speciesgrain.cpp
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

#include "classes/speciesgrain.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "base/ptable.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for SpeciesGrain. 
 */
SpeciesGrain::SpeciesGrain() : ListItem<SpeciesGrain>()
{
	parent_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for SpeciesGrain. 
 */
SpeciesGrain::~SpeciesGrain()
{
}

/*
// Basic Character
*/

/*!
 * \brief Set parent Species
 */
void SpeciesGrain::setParent(Species* parent)
{
	parent_ = parent;
}

/*!
 * \brief Return parent Species
 */
Species *SpeciesGrain::parent() const
{
	return parent_;
}

/*!
 * \brief Set name of SpeciesGrain
 */
void SpeciesGrain::setName(const char* s)
{
	name_ = s;
}

/*!
 * \brief Return name of SpeciesGrain
 */
const char* SpeciesGrain::name() const
{
	return name_.get();
}

/*
// Atoms
*/

/*!
 * \brief Search current list for Atom
 */
bool SpeciesGrain::containsAtom(SpeciesAtom* i) const
{
	return atoms_.contains(i);
}

/*!
 * \brief Add specified Atom to list
 */
void SpeciesGrain::addAtom(SpeciesAtom* i)
{
	atoms_.add(i);
	i->setGrain(this);
}

/*!
 * \brief Remove specified Atom from list
 */
void SpeciesGrain::removeAtom(SpeciesAtom* i)
{
	atoms_.remove(i);
	i->setGrain(NULL);
}

/*!
 * \brief Return number of Atoms in the list
 */
int SpeciesGrain::nAtoms() const
{
	return atoms_.nItems();
}

/*!
 * \brief Return first Atom reference in list
 */
RefListItem<SpeciesAtom,int>* SpeciesGrain::atoms() const
{
	return atoms_.first();
}

/*!
 * \brief Return nth Atom reference in list
 */
RefListItem<SpeciesAtom,int>* SpeciesGrain::atom(int n)
{
	return atoms_[n];
}

/*!
 * \brief Return empirical name of SpeciesGrain from constituent Atoms
 * \details Creates and returns a name for the SpeciesGrain which corresponds to the empirical formula of its Atoms
 */
const char* SpeciesGrain::nameFromAtoms()
{
	PeriodicTable::resetEmpiricalFormula();
	for (RefListItem<SpeciesAtom,int>* ri = atoms_.first(); ri != NULL; ri = ri->next) PeriodicTable::addToEmpirical(ri->item->element());
	return PeriodicTable::empiricalFormula();
}

/*
 * Connections
 */

/*!
 * \brief Clear all intra- and inter-Grain terms
 */
void SpeciesGrain::clearConnections()
{
	internalBonds_.clear();
	internalAngles_.clear();
	bondConnections_.clear();
	angleConnections_.clear();
}

/*!
 * \brief Add intra-Grain Bond
 */
void SpeciesGrain::addInternalBond(SpeciesBond* b)
{
	internalBonds_.add(b);
}

/*!
 * \brief Return first local intra-Grain Bond
 */
RefListItem<SpeciesBond, int>* SpeciesGrain::internalBonds() const
{
	return internalBonds_.first();
}

/*!
 * \brief Add intra-Grain Angle
 */
void SpeciesGrain::addInternalAngle(SpeciesAngle* a)
{
	internalAngles_.add(a);
}

/*!
 * \brief Return first local intra-Grain Angle
 */
RefListItem<SpeciesAngle, int>* SpeciesGrain::internalAngles() const
{
	return internalAngles_.first();
}

/*!
 * \brief Add Bond connection
 */
void SpeciesGrain::addBondConnection(SpeciesBond* b)
{
	bondConnections_.add(b);
}

/*!
 * \brief Return first Bond connection
 */
RefListItem<SpeciesBond, int>* SpeciesGrain::bondConnections() const
{
	return bondConnections_.first();
}

/*!
 * \brief Add Angle connection
 */
void SpeciesGrain::addAngleConnection(SpeciesAngle* a)
{
	angleConnections_.add(a);
}

/*!
 * \brief Return first Angle connection
 */
RefListItem<SpeciesAngle, int>* SpeciesGrain::angleConnections() const
{
	return angleConnections_.first();
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool SpeciesGrain::broadcast(const List<SpeciesAtom>& atoms, const List<SpeciesBond>& bonds, const List<SpeciesAngle>& angles)
{
#ifdef PARALLEL
	int n, count, index;

	// Name
	if (!Comm.broadcast(name_)) return false;
	
	// Atoms
	count = atoms_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = atoms_.item(n)->item->index();
		if (!Comm.broadcast(&index, 1)) return false;
		if (Comm.slave()) addAtom(atoms.item(index));
	}

	// Intra-grain bonds / angles
	count = internalBonds_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = bonds.indexOf(internalBonds_.item(n)->item);
		if (!Comm.broadcast(&index, 1)) return false;
		if (Comm.slave()) addInternalBond(bonds.item(index));
	}
	count = internalAngles_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = angles.indexOf(internalAngles_.item(n)->item);
		if (!Comm.broadcast(&index, 1)) return false;
		if (Comm.slave()) addInternalAngle(angles.item(index));
	}

	// Inter-grain (connective) bonds / angles
	count = bondConnections_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = bonds.indexOf(bondConnections_.item(n)->item);
		if (!Comm.broadcast(&index, 1)) return false;
		if (Comm.slave()) addBondConnection(bonds.item(index));
	}
	count = angleConnections_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.master()) index = angles.indexOf(angleConnections_.item(n)->item);
		if (!Comm.broadcast(&index, 1)) return false;
		if (Comm.slave()) addAngleConnection(angles.item(index));
	}
#endif
	return true;
}

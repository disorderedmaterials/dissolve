/*
	*** Species Definition
	*** src/classes/species.cpp
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

#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/lineparser.h"
#include "base/comms.h"
#include <string.h>
#include <base/sysfunc.h>

// Constructor
Species::Species() : ListItem<Species>()
{
	highlightedIsotopologue_ = NULL;
	highlightedGrain_ = NULL;
}

// Destructor
Species::~Species()
{
}

// Clear Data
void Species::clear()
{
	highlightedIsotopologue_ = NULL;
	isotopologues_.clear();
	highlightedGrain_ = NULL;
	grains_.clear();
	angles_.clear();
	bonds_.clear();
	atoms_.clear();
}

/*
 * Basic Information
 */

// Set name of the Species
void Species::setName(const char* name)
{
	name_ = name;
}

// Return the name of the Species
const char* Species::name() const
{
	return name_.get();
}

/*
 * Check setup
 */
bool Species::checkSetup(const List<AtomType>& atomTypes)
{
	int nErrors = 0;
	// Must have at least one atom...
	if (atoms_.nItems() == 0)
	{
		msg.error("Species contains no Atoms.\n");
		return false;
	}
	
	/* Check AtomTypes */
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if (i->atomType() == NULL)
		{
			msg.error("Atom %i (%s) has no associated AtomType.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		else if (!atomTypes.contains(i->atomType()))
		{
			msg.error("Atom %i (%s) references a non-existent AtomType.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
	}
	if (nErrors > 0) return false;

	/* GrainDefinitions */
	/* Each Atom must be in exactly one GrainDefinition */
	RefList<SpeciesAtom,int> grainCount(atoms_, 0);
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next)
		{
			RefListItem<SpeciesAtom,int>* rj = grainCount.contains(ri->item);
			if (rj == NULL)
			{
				msg.error("GrainDefinition '%s' references a non-existent Atom.\n", sg->name());
				++nErrors;
			}
			else ++rj->data;
		}
	}
	for (RefListItem<SpeciesAtom,int>* ri = grainCount.first(); ri != NULL; ri = ri->next)
	{
		if (ri->data == 0)
		{
			msg.error("SpeciesAtom %i (%s) is not present in any GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol());
			++nErrors;
		}
		else if (ri->data > 1)
		{
			msg.error("SpeciesAtom %i (%s) is present in more than one (%i) GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol(), ri->data);
			++nErrors;
		}
	}
	if (nErrors > 0) return false;

	/* Check IntraMolecular Data */
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if ((i->nBonds() == 0) && (atoms_.nItems() > 1))
		{
			msg.error("SpeciesAtom %i (%s) participates in no Bonds, but is part of a multi-atom Species.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		
		/* Check each Bond for two-way consistency */
		for (RefListItem<SpeciesBond,int>* ri = i->bonds(); ri != NULL; ri = ri->next)
		{
			SpeciesAtom* j = ri->item->partner(i);
			if (!j->hasBond(i))
			{
				msg.error("SpeciesAtom %i references a Bond to SpeciesAtom %i, but SpeciesAtom %i does not.\n", i->userIndex(), j->userIndex(), j->userIndex());
				++nErrors;
			}
		}
	}
	if (nErrors > 0) return false;

	/* Check Isotopologues */
	if (isotopologues_.nItems() == 0)
	{
		msg.error("No Isotopologues defined in Species.\n");
		++nErrors;
	}
	else for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next)
	{
		for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
		{
			if (!atomTypes.contains(ri->item))
			{
				msg.error("Isotopologue '%s' refers to an unknown AtomType.\n", iso->name());
				++nErrors;
			}
			else if (ri->data == NULL)
			{
				msg.error("Isotopologue '%s' does not refer to an elemental Isotope for AtomType '%s'.\n", iso->name(), ri->item->name());
				++nErrors;
			}
			else if (!PeriodicTable::element(ri->item->element()).hasIsotope(ri->data->A()))
			{
				msg.error("Isotopologue '%s' does not refer to a suitable Isotope for AtomType '%s'.\n", iso->name(), ri->item->name());
				++nErrors;
			}
		}
	}

	return (nErrors == 0);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Species::broadcast(const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	int n, count;

	// Send basic information
	Comm.broadcast(name_);
	Comm.broadcast(&relativePopulation_, 1);

	// Atoms
	msg.printVerbose("[MPI] Broadcasting Atoms...\n");
	count = atoms_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addAtom();
		atoms_[n]->broadcast(atomTypes);
	}
	
	// Bonds
	msg.printVerbose("[MPI] Broadcasting bonds...\n");
	count = bonds_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addBond() function
		if (Comm.slave())
		{
			SpeciesBond* b = bonds_.add();
			b->setParent(this);
			b->broadcast(atoms_);
		}
		else bonds_[n]->broadcast(atoms_);
	}

	// Angles
	msg.printVerbose("[MPI] Broadcasting angles...\n");
	count = angles_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addAngle() function
		if (Comm.slave())
		{
			SpeciesAngle* a = angles_.add();
			a->setParent(this);
			a->broadcast(atoms_);
		}
		else angles_[n]->broadcast(atoms_);
	}
	
	// Grains
	msg.printVerbose("[MPI] Broadcasting grains...\n");
	count = grains_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addGrain();
		grains_[n]->broadcast(atoms_, bonds_, angles_);
	}

	// Isotopologues
	msg.printVerbose("[MPI] Broadcasting isotopologues...\n");
	count = isotopologues_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	msg.printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addIsotopologue();
		isotopologues_[n]->broadcast(atomTypes);
	}
#endif
	return true;
}


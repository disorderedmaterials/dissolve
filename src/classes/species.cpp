/*
	*** Species Definition
	*** src/classes/species.cpp
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

#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include <string.h>
#include <base/sysfunc.h>

// Constructor
Species::Species() : ListItem<Species>()
{
}

// Destructor
Species::~Species()
{
}

// Clear Data
void Species::clear()
{
	isotopologues_.clear();
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
		Messenger::error("Species contains no Atoms.\n");
		return false;
	}
	
	/* Check AtomTypes */
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if (i->atomType() == NULL)
		{
			Messenger::error("Atom %i (%s) has no associated AtomType.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		else if (!atomTypes.contains(i->atomType()))
		{
			Messenger::error("Atom %i (%s) references a non-existent AtomType.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
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
				Messenger::error("GrainDefinition '%s' references a non-existent Atom.\n", sg->name());
				++nErrors;
			}
			else ++rj->data;
		}
	}
	for (RefListItem<SpeciesAtom,int>* ri = grainCount.first(); ri != NULL; ri = ri->next)
	{
		if (ri->data == 0)
		{
			Messenger::error("SpeciesAtom %i (%s) is not present in any GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol());
			++nErrors;
		}
		else if (ri->data > 1)
		{
			Messenger::error("SpeciesAtom %i (%s) is present in more than one (%i) GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol(), ri->data);
			++nErrors;
		}
	}
	if (nErrors > 0) return false;

	/* Check IntraMolecular Data */
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next)
	{
		if ((i->nBonds() == 0) && (atoms_.nItems() > 1))
		{
			Messenger::error("SpeciesAtom %i (%s) participates in no Bonds, but is part of a multi-atom Species.\n", i->userIndex(), PeriodicTable::element(i->element()).symbol());
			++nErrors;
		}
		
		/* Check each Bond for two-way consistency */
		for (RefListItem<SpeciesBond,int>* ri = i->bonds(); ri != NULL; ri = ri->next)
		{
			SpeciesAtom* j = ri->item->partner(i);
			if (!j->hasBond(i))
			{
				Messenger::error("SpeciesAtom %i references a Bond to SpeciesAtom %i, but SpeciesAtom %i does not.\n", i->userIndex(), j->userIndex(), j->userIndex());
				++nErrors;
			}
		}
	}
	if (nErrors > 0) return false;

	/* Check Isotopologues */
	if (isotopologues_.nItems() == 0)
	{
		Messenger::error("No Isotopologues defined in Species.\n");
		++nErrors;
	}
	else for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next)
	{
		for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
		{
			if (!atomTypes.contains(ri->item))
			{
				Messenger::error("Isotopologue '%s' refers to an unknown AtomType.\n", iso->name());
				++nErrors;
			}
			else if (ri->data == NULL)
			{
				Messenger::error("Isotopologue '%s' does not refer to an elemental Isotope for AtomType '%s'.\n", iso->name(), ri->item->name());
				++nErrors;
			}
			else if (!PeriodicTable::element(ri->item->element()).hasIsotope(ri->data->A()))
			{
				Messenger::error("Isotopologue '%s' does not refer to a suitable Isotope for AtomType '%s'.\n", iso->name(), ri->item->name());
				++nErrors;
			}
		}
	}

	return (nErrors == 0);
}

// Print Species information
void Species::print()
{
	Messenger::print("  Atoms:\n");
	Messenger::print("      ID   El  Type (ID)        X           Y           Z      \n");
	Messenger::print("    -------------------------------------------------------------\n");
	for (int n=0; n<nAtoms(); ++n)
	{
		SpeciesAtom* i = atoms_[n];
		Messenger::print("    %4i  %3s  %4s (%2i)  %12.4e  %12.4e  %12.4e\n", n+1, PeriodicTable::element(i->element()).symbol(), i->atomType()->name(), i->atomType()->index(), i->r().x, i->r().y, i->r().z);
	}


	if (nBonds() > 0)
	{
		Messenger::print("\n  Bonds:\n");
		Messenger::print("      I     J        Eq          ForceK   \n");
		Messenger::print("    --------------------------------------\n");
		for (int n=0; n<nBonds(); ++n)
		{
			SpeciesBond* b = bonds_[n];
			Messenger::print("   %4i  %4i  %12.4e  %12.4e\n", b->indexI()+1, b->indexJ()+1, b->equilibrium(), b->forceConstant());
		}
	}

	if (nAngles() > 0)
	{
		Messenger::print("\n  Angles:\n");
		Messenger::print("      I     J     K        Eq          ForceK   \n");
		Messenger::print("    --------------------------------------------\n");
		for (int n=0; n<nAngles(); ++n)
		{
			SpeciesAngle* a = angles_[n];
			Messenger::print("   %4i  %4i  %4i  %12.4e  %12.4e\n", a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->equilibrium(), a->forceConstant());
		}
	}

	Messenger::print("\n  Grains:\n");
	for (int n=0; n<nGrains(); ++n)
	{
		SpeciesGrain* grain = grains_[n];
		CharString grainAtoms;
		for (int m=0; m<grain->nAtoms(); ++m) grainAtoms.strcatf("%4i ", grain->atom(m)->item->userIndex());
		Messenger::print("  %4i  '%s'\n", n+1, grain->name());
		Messenger::print("       %2i atoms: %s\n", grain->nAtoms(), grainAtoms.get());
	}
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Species::broadcast(ProcessPool& procPool, const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	int n, count;

	// Send basic information
	procPool.broadcast(name_);

	// Atoms
	Messenger::printVerbose("[MPI] Broadcasting Atoms...\n");
	count = atoms_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (procPool.isSlave()) addAtom();
		atoms_[n]->broadcast(procPool, atomTypes);
	}
	
	// Bonds
	Messenger::printVerbose("[MPI] Broadcasting bonds...\n");
	count = bonds_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addBond() function
		if (procPool.isSlave())
		{
			SpeciesBond* b = bonds_.add();
			b->setParent(this);
			b->broadcast(procPool, atoms_);
		}
		else bonds_[n]->broadcast(procPool, atoms_);
	}

	// Angles
	Messenger::printVerbose("[MPI] Broadcasting angles...\n");
	count = angles_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		// This is not ideal, since we are not using the safer addAngle() function
		if (procPool.isSlave())
		{
			SpeciesAngle* a = angles_.add();
			a->setParent(this);
			a->broadcast(procPool, atoms_);
		}
		else angles_[n]->broadcast(procPool, atoms_);
	}
	
	// Grains
	Messenger::printVerbose("[MPI] Broadcasting grains...\n");
	count = grains_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (procPool.isSlave()) addGrain();
		grains_[n]->broadcast(procPool, atoms_, bonds_, angles_);
	}

	// Isotopologues
	Messenger::printVerbose("[MPI] Broadcasting isotopologues...\n");
	count = isotopologues_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (procPool.isSlave()) addIsotopologue();
		isotopologues_[n]->broadcast(procPool, atomTypes);
	}
#endif
	return true;
}


/*
	*** Species Definition
	*** src/classes/species.cpp
	Copyright T. Youngs 2012-2018

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
	RefList<SpeciesAtom,int> grainCount;
	for (SpeciesAtom* sa = atoms_.first(); sa != NULL; sa = sa->next) grainCount.add(sa, 0);
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
		if (ri->data > 1)
		{
			Messenger::error("SpeciesAtom %i (%s) is present in more than one (%i) GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol(), ri->data);
			++nErrors;
		}
// 		else if (ri->data == 0)
// 		{
// 			Messenger::error("SpeciesAtom %i (%s) is not present in any GrainDefinition.\n", ri->item->userIndex(), PeriodicTable::element(ri->item->element()).symbol());
// 			++nErrors;
// 		}
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
		Messenger::print("      I     J    Form          Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
		{
			if (b->masterParameters()) Messenger::print("   %4i  %4i    @%-12s\n", b->indexI()+1, b->indexJ()+1, b->masterParameters()->name());
			else
			{
				CharString s("   %4i  %4i    %-12s", b->indexI()+1, b->indexJ()+1, SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()));
				for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", b->parameter(n));
				Messenger::print("%s\n", s.get());
			}
		}
	}

	if (nAngles() > 0)
	{
		Messenger::print("\n  Angles:\n");
		Messenger::print("      I     J     K    Form          Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
		{
			if (a->masterParameters()) Messenger::print("   %4i  %4i  %4i    @%-12s\n", a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->masterParameters()->name());
			else
			{
				CharString s("   %4i  %4i  %4i    %-12s", a->indexI()+1, a->indexJ()+1, a->indexK()+1, SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()));
				for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", a->parameter(n));
				Messenger::print("%s\n", s.get());
			}
		}
	}

	if (nTorsions() > 0)
	{
		Messenger::print("\n  Torsions:\n");
		Messenger::print("      I     J     K     L    Form          Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (SpeciesTorsion* t = torsions_.first(); t != NULL; t = t->next)
		{
			if (t->masterParameters()) Messenger::print("   %4i  %4i  %4i  %4i    %-12s", t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1, t->masterParameters()->name());
			else
			{
				CharString s("   %4i  %4i  %4i  %4i    %-12s", t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1, SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()));
				for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", t->parameter(n));
				Messenger::print("%s\n", s.get());
			}
		}
	}

	if (nGrains() > 0)
	{
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
}

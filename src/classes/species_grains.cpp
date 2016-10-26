/*
	*** Species Definition - Grains
	*** src/classes/species_grains.cpp
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

#include "classes/species.h"
#include "classes/atomtype.h"
#include <string.h>
#include <base/sysfunc.h>

// Update grains after change
void Species::updateGrains()
{
	// Construct a list of Atom references as we go along, so we can check for duplicates
	RefList<SpeciesAtom,SpeciesGrain*> refAtoms;
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		// Loop over Atoms in GrainDefinition
		RefListItem<SpeciesAtom,int>* ri = sg->atoms(), *next;
		RefListItem<SpeciesAtom,SpeciesGrain*>* rj;
		while (ri != NULL)
		{
			next = ri->next;
			rj = refAtoms.contains(ri->item);
			if (rj)
			{
				sg->removeAtom(rj->item);
				Messenger::print("Removed Atom %i from GrainDefinition '%s' since it already exists in '%s'.\n", rj->item->userIndex(), sg->name(), rj->data->name());
			}
			else refAtoms.add(ri->item, sg);

			ri = next;
		}
	}
	
	// Check for empty GrainDefinitions
	SpeciesGrain* sg = grains_.first(), *next;
	while (sg != NULL)
	{
		next = sg->next;
		if (sg->nAtoms() == 0)
		{
			Messenger::print("Removing grain '%s' since it no longer contains any atoms.\n", sg->name());
			removeGrain(sg);
		}
		sg = next;
	}
}

// Add default GrainDefinition (i.e. one which contains all atoms) for this Species 
void Species::addDefaultGrain()
{
	SpeciesGrain* sg = grains_.add();
	
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) sg->addAtom(i);
	
	sg->setName(sg->nameFromAtoms());

	// Locate inter/intra-grain terms
	identifyInterGrainTerms();
}

// Automatically determine Grains based on chemical connectivity
void Species::autoAddGrains()
{
	grains_.clear();
	
	// Make a list of bonds for each atom
	RefList<SpeciesBond,int> bondList[atoms_.nItems()];
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		bondList[b->indexI()].add(b,1);
		bondList[b->indexJ()].add(b,1);
	}
	
	// Now loop over Atoms
	SpeciesGrain* sg;
	int n, m, *group = new int[atoms_.nItems()];
	for (n=0; n<atoms_.nItems(); ++n) group[n] = 0;
	for (n=0; n<atoms_.nItems(); ++n)
	{
		if (group[n] == 1) continue;
		else if (bondList[n].nItems() == 0)
		{
			group[n] = 1;
			sg = grains_.add();
			sg->setParent(this);
			sg->addAtom(atoms_[n]);
		}
		else if (bondList[n].nItems() == 1) continue;
		else
		{
			sg = grains_.add();
			sg->setParent(this);
			group[n] = 1;
			sg->addAtom(atoms_[n]);
			for (RefListItem<SpeciesBond,int>* ri = bondList[n].first(); ri != NULL; ri = ri->next)
			{
				m = ri->item->indexI() == n ? ri->item->indexJ() : ri->item->indexI();
				if ((bondList[m].nItems() != 1) || (group[m] == 1)) continue;
				group[m] = 1;
				sg->addAtom(atoms_[m]);
			}
		}
	}
	
	// Name each grain....
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next) sg->setName(uniqueGrainName(sg->nameFromAtoms()));

	// Locate inter/intra-grain terms
	identifyInterGrainTerms();
}

// Add new GrainDefinition for this Species
SpeciesGrain* Species::addGrain()
{
	SpeciesGrain* sg = grains_.add();

	return sg;
}

// Remove GrainDefinition specified
void Species::removeGrain(SpeciesGrain* sg)
{
	if (sg == NULL) Messenger::error("NULL_POINTER - NULL GrainDefinition passed to Species::removeGrainDefinition().\n");
	else if (grains_.contains(sg))
	{
		if (highlightedGrain_ == sg) highlightedGrain_ = NULL;
		grains_.remove(sg);
		Messenger::print("Removed grain from Species '%s'.\n", name_.get());
	}
	else Messenger::print("BAD_REMOVE - Can't remove specified grain from Species '%s' since it doesn't exist.\n", name_.get());
}

// Return number of GrainDefinitions present for this Species
int Species::nGrains() const
{
	return grains_.nItems();
}

// Return first GrainDefinition in list
SpeciesGrain* Species::grains() const
{
	return grains_.first();
}

// Return nth GrainDefinition in list
SpeciesGrain* Species::grain(int n)
{
	return grains_[n];
}

// Add Atom to GrainDefinition
void Species::addAtomToGrain(SpeciesAtom* i, SpeciesGrain* gd)
{
	// Check for presence of Atom in another definition - if it exists there, remove it...
	for (SpeciesGrain* def = grains_.first(); def != NULL; def = def->next)
	{
		if (def->containsAtom(i))
		{
			// Is this the target GrainDefinition?
			if (def == gd)
			{
				Messenger::print("Warning: Won't add duplicate of atom %i to grain '%s' in Species '%s'.\n", i->userIndex(), gd->name(), name());
				return;
			}
			def->removeAtom(i);
			Messenger::print("Removed atom %i from grain '%s' so it can be added to '%s'.\n", i->userIndex(), def->name(), gd->name());
			break;
		}
	}
	gd->addAtom(i);
}

// Generate unique GrainDefinition name with base name provided
const char* Species::uniqueGrainName(const char* base, SpeciesGrain* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	SpeciesGrain* sg;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		if (sg == exclude) continue;
		if (strcmp(baseName, sg->name()) == 0) highest = 0;
		else if (strcmp(baseName, DUQSys::beforeLastChar(sg->name(),'_')) == 0) highest = atoi(DUQSys::afterLastChar(sg->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

// Order atoms within grains
void Species::orderAtomsWithinGrains()
{
	/*
	 * Reorders atoms within the Species such that all grains contain a consecutive set of atom indices, and that indices
	 * present within one grain are *higher* than those in preceeding Grains in the list.
	 */

	// Loop over Grains.
	// The variable 'index' will represent the target position in the Atom list of the next Atom to be considered within a grain.
	int n, index = 0, oldIndex;
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next)
	{
		for (n=0; n<sg->nAtoms(); ++n)
		{
			oldIndex = atoms_.indexOf(sg->atom(n)->item);
			atoms_.move(oldIndex, index - oldIndex);
			++index;
		}
	}
	
	// Now need to renumber Atoms
	index = 0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) i->setIndex(index++);
}

// Set highlighted grain
void Species::setHighlightedGrain(SpeciesGrain* sg)
{
	highlightedGrain_ = sg;
}

// Return highlighted grain
SpeciesGrain* Species::highlightedGrain()
{
	return highlightedGrain_;
}

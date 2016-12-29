/*
	*** dUQ - AtomTypes
	*** src/main/atomtypes.cpp
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

#include "main/duq.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/sysfunc.h"
#include <string.h>

/*
 * Atom Types
 */

// Add AtomType definition
AtomType* DUQ::addAtomType(int el)
{
	AtomType* at = atomTypes_.add();

	// Create a suitable name...
	at->setName(uniqueAtomTypeName(PeriodicTable::element(el).symbol()));
	at->setElement(el);

	return at;
}

// Remove AtomType definition
void DUQ::removeAtomType(AtomType* at)
{
	// Nullify all Atoms using this AtomType
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next) if (i->atomType() == at) i->setAtomType(NULL);
	}
	atomTypes_.remove(at);

	updateAtomTypes();
}

// Return number of AtomTypes in list
int DUQ::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return first AtomType in list
AtomType* DUQ::atomTypes() const
{
	return atomTypes_.first();
}

// Return nth AtomType in list
AtomType* DUQ::atomType(int n)
{
	return atomTypes_[n];
}

// Return first AtomType for element specified
AtomType* DUQ::atomTypeForElement(int el) const
{
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) if (at->element() == el) return at;
	return NULL;
}

// Update AtomTypes definitions
void DUQ::updateAtomTypes()
{
	// Loop over Atoms in all Species - check AtomType availability and validity for each
	AtomType* at;
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
		{
			// Is current AtomType definition valid?
			if (i->atomType() != NULL)
			{
				// Check it exists...
				if (!atomTypes_.contains(i->atomType())) i->setAtomType(NULL);
				else
				{
					// Check elements
					if (i->element() == i->atomType()->element()) continue;
					else i->setAtomType(NULL);
				}
			}
			
			// Either not valid, or nothing present.
			// Does an AtomType definition exist for this element?
			at = atomTypeForElement(i->element());
			if (!at)
			{
				// Create a suitable AtomType
				at = atomTypes_.add();
				at->setElement(i->element());
				at->setParameters(PeriodicTable::element(i->element()).parameters());
				at->setName(uniqueAtomTypeName(PeriodicTable::element(i->element()).symbol(), at));
			}
			i->setAtomType(at);
		}
	}
	
	// Step through AtomType definitions and make sure all have a valid set of Parameters
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
	{
		if (!at->parameters()) at->setParameters(PeriodicTable::element(at->element()).parameters());
	}
}

// Generate unique AtomType name with base name provided
const char* DUQ::uniqueAtomTypeName(const char* base, AtomType* exclude) const
{
	static CharString uniqueName;
	CharString baseName = base;
	AtomType* at;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (at = atomTypes_.first(); at != NULL; at = at->next)
	{
		if ( at == exclude) continue;
		if (strcmp(baseName, at->name()) == 0) highest = 0;
		else if (strcmp(baseName,DUQSys::beforeLastChar(at->name(),'_')) == 0) highest = atoi(DUQSys::afterLastChar(at->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

// Search for AtomType by name
AtomType* DUQ::findAtomType(const char* name) const
{
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) if (strcmp(at->name(),name) == 0) return at;
	return NULL;
}

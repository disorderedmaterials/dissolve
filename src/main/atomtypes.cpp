/*
	*** Dissolve - AtomTypes
	*** src/main/atomtypes.cpp
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/dissolve.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/sysfunc.h"

/*
 * Atom Types
 */

// Add AtomType with specified Element
AtomType* Dissolve::addAtomType(Element* el)
{
	AtomType* at = atomTypes_.add();

	// Create a suitable name...
	at->setName(uniqueAtomTypeName(el->symbol()));
	at->setElement(el);
	at->setIndex(atomTypes_.nItems() - 1);

	setUp_ = false;

	return at;
}

// Return number of AtomTypes in list
int Dissolve::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return first AtomType in list
AtomType* Dissolve::atomTypes() const
{
	return atomTypes_.first();
}

// Return AtomTypes list
const List<AtomType>& Dissolve::atomTypeList() const
{
	return atomTypes_;
}

// Return nth AtomType in list
AtomType* Dissolve::atomType(int n)
{
	return atomTypes_[n];
}

// Generate unique AtomType name with base name provided
const char* Dissolve::uniqueAtomTypeName(const char* base, AtomType* exclude) const
{
	CharString baseName = base;
	AtomType* at;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (at = atomTypes_.first(); at != NULL; at = at->next)
	{
		if ( at == exclude) continue;
		if (strcmp(baseName, at->name()) == 0) highest = 0;
		else if (strcmp(baseName,DissolveSys::beforeLastChar(at->name(),'_')) == 0) highest = atoi(DissolveSys::afterLastChar(at->name(), '_'));
	}

	static CharString uniqueName;
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

// Search for AtomType by name
AtomType* Dissolve::findAtomType(const char* name) const
{
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) if (strcmp(at->name(),name) == 0) return at;
	return NULL;
}

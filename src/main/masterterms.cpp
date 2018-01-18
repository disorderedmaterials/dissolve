/*
	*** dUQ - Master Terms
	*** src/main/masterterms.cpp
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

#include "main/duq.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"

// Add new master Bond definition
SpeciesBond* DUQ::addMasterBond(const char* name)
{
	// Check for existence of master Bond already
	if (hasMasterBond(name))
	{
		Messenger::error("Refused to add a new master Bond named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Bond
	SpeciesBond* b = masterBonds_.add();
	b->setName(name);

	return b;
}

// Return number of master Bonds in list
int DUQ::nMasterBonds() const
{
	return masterBonds_.nItems();
}

// Return list of master Bonds
SpeciesBond* DUQ::masterBonds() const
{
	return masterBonds_.first();
}

// Return nth master Bond
SpeciesBond* DUQ::masterBond(int n)
{
	return masterBonds_[n];
}

// Return whether named master Bond exists
SpeciesBond* DUQ::hasMasterBond(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (SpeciesBond* b = masterBonds_.first(); b != NULL; b = b->next) if (DUQSys::sameString(trimmedName, b->name())) return b;
	return NULL;
}

// Add new master Angle definition
SpeciesAngle* DUQ::addMasterAngle(const char* name)
{
	// Check for existence of master Angle already
	if (hasMasterAngle(name))
	{
		Messenger::error("Refused to add a new master Angle named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Angle
	SpeciesAngle* a = masterAngles_.add();
	a->setName(name);

	return a;
}

// Return number of master Angles in list
int DUQ::nMasterAngles() const
{
	return masterAngles_.nItems();
}

// Return list of master Angles
SpeciesAngle* DUQ::masterAngles() const
{
	return masterAngles_.first();
}

// Return nth master Angle
SpeciesAngle* DUQ::masterAngle(int n)
{
	return masterAngles_[n];
}

// Return whether named master Angle exists
SpeciesAngle* DUQ::hasMasterAngle(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (SpeciesAngle* a = masterAngles_.first(); a != NULL; a = a->next) if (DUQSys::sameString(trimmedName, a->name())) return a;
	return NULL;
}

// Add new master Torsion definition
SpeciesTorsion* DUQ::addMasterTorsion(const char* name)
{
	// Check for existence of master Torsion already
	if (hasMasterTorsion(name))
	{
		Messenger::error("Refused to add a new master Torsion named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Torsion
	SpeciesTorsion* t = masterTorsions_.add();
	t->setName(name);

	return t;
}

// Return number of master Torsions in list
int DUQ::nMasterTorsions() const
{
	return masterTorsions_.nItems();
}

// Return list of master Torsions
SpeciesTorsion* DUQ::masterTorsions() const
{
	return masterTorsions_.first();
}

// Return nth master Torsion
SpeciesTorsion* DUQ::masterTorsion(int n)
{
	return masterTorsions_[n];
}

// Return whether named master Torsion exists
SpeciesTorsion* DUQ::hasMasterTorsion(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (SpeciesTorsion* t = masterTorsions_.first(); t != NULL; t = t->next) if (DUQSys::sameString(trimmedName, t->name())) return t;
	return NULL;
}

/*
	*** Dissolve - Master Terms
	*** src/main/masterterms.cpp
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
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"

// Add new master Bond parameters
MasterIntra* Dissolve::addMasterBond(const char* name)
{
	// Check for existence of master Bond already
	if (hasMasterBond(name))
	{
		Messenger::error("Refused to add a new master Bond named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Bond
	MasterIntra* b = masterBonds_.add();
	b->setName(name);
	b->setType(SpeciesIntra::IntramolecularBond);

	return b;
}

// Return number of master Bond parameters in list
int Dissolve::nMasterBonds() const
{
	return masterBonds_.nItems();
}

// Return list of master Bond parameters
const List<MasterIntra>& Dissolve::masterBonds() const
{
	return masterBonds_;
}

// Return nth master Bond parameters
MasterIntra* Dissolve::masterBond(int n)
{
	return masterBonds_[n];
}

// Return whether named master Bond parameters exist
MasterIntra* Dissolve::hasMasterBond(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next) if (DissolveSys::sameString(trimmedName, b->name())) return b;
	return NULL;
}

// Add new master Angle parameters
MasterIntra* Dissolve::addMasterAngle(const char* name)
{
	// Check for existence of master Angle already
	if (hasMasterAngle(name))
	{
		Messenger::error("Refused to add a new master Angle named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Angle
	MasterIntra* a = masterAngles_.add();
	a->setName(name);
	a->setType(SpeciesIntra::IntramolecularAngle);

	return a;
}

// Return number of master Angle parameters in list
int Dissolve::nMasterAngles() const
{
	return masterAngles_.nItems();
}

// Return list of master Angle parameters
const List<MasterIntra>& Dissolve::masterAngles() const
{
	return masterAngles_;
}

// Return nth master Angle parameters
MasterIntra* Dissolve::masterAngle(int n)
{
	return masterAngles_[n];
}

// Return whether named master Angle parameters exist
MasterIntra* Dissolve::hasMasterAngle(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next) if (DissolveSys::sameString(trimmedName, a->name())) return a;
	return NULL;
}

// Add new master Torsion parameters
MasterIntra* Dissolve::addMasterTorsion(const char* name)
{
	// Check for existence of master Torsion already
	if (hasMasterTorsion(name))
	{
		Messenger::error("Refused to add a new master Torsion named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Torsion
	MasterIntra* t = masterTorsions_.add();
	t->setName(name);
	t->setType(SpeciesIntra::IntramolecularTorsion);

	return t;
}

// Return number of master Torsion parameters in list
int Dissolve::nMasterTorsions() const
{
	return masterTorsions_.nItems();
}

// Return list of master Torsion parameters
const List<MasterIntra>& Dissolve::masterTorsions() const
{
	return masterTorsions_;
}

// Return nth master Torsion parameters
MasterIntra* Dissolve::masterTorsion(int n)
{
	return masterTorsions_[n];
}

// Return whether named master Torsion parameters exist
MasterIntra* Dissolve::hasMasterTorsion(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next) if (DissolveSys::sameString(trimmedName, t->name())) return t;
	return NULL;
}

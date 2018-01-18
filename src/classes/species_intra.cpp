/*
	*** Species Definition - Intramolecular Terms
	*** src/classes/species_intra.cpp
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
#include "classes/box.h"
#include "base/sysfunc.h"

// Add new Bond definition (from Atoms*)
SpeciesBond* Species::addBond(SpeciesAtom* i, SpeciesAtom* j)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addBond.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addBond.\n", name_.get());
		return NULL;
	}

	// Check for existence of Bond already
	if (hasBond(i, j))
	{
		Messenger::warn("Refused to add a new Bond between atoms %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), name_.get());
		return NULL;
	}
	
	// OK to add new Bond
	SpeciesBond* b = bonds_.add();
	b->setParent(this);
	b->setAtoms(i, j);
	i->addBond(b);
	j->addBond(b);

	return b;
}

// Add new Bond definition
SpeciesBond* Species::addBond(int i, int j)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}

	return addBond(atoms_[i], atoms_[j]);
}

// Return number of bonds in list
int Species::nBonds() const
{
	return bonds_.nItems();
}

// Return list of bonds
SpeciesBond* Species::bonds() const
{
	return bonds_.first();
}

// Return nth bond
SpeciesBond* Species::bond(int n)
{
	return bonds_[n];
}

// Return whether bond between atoms exists
SpeciesBond* Species::hasBond(SpeciesAtom* i, SpeciesAtom* j) const
{
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next) if (b->matches(i, j)) return b;
	return NULL;
}

// Add new angle definition (from supplied atom pointers)
SpeciesAngle* Species::addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'k' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}

	// Check for existence of Angle already
	if (hasAngle(i, j, k))
	{
		Messenger::warn("Refused to add a new Angle between atoms %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), k->userIndex(), name_.get());
		return NULL;
	}

	// OK to add new angle
	SpeciesAngle* a = angles_.add();
	a->setParent(this);
	a->setAtoms(i, j, k);

	return a;
}

// Add new angle definition
SpeciesAngle* Species::addAngle(int i, int j, int k)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}
	return addAngle(atoms_[i], atoms_[j], atoms_[k]);
}

// Return number of angles in list
int Species::nAngles() const
{
	return angles_.nItems();
}

// Return list of angles
SpeciesAngle* Species::angles() const
{
	return angles_.first();
}

// Return nth angle
SpeciesAngle* Species::angle(int n)
{
	return angles_[n];
}

// Return whether angle between atoms exists
bool Species::hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const
{
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next) if (a->matches(i, j, k)) return true;
	return false;
}

// Add new torsion definition (from supplied atom pointers)
SpeciesTorsion* Species::addTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'k' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(l))
	{
		Messenger::print("BAD_OWNERSHIP - Atom 'l' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}

	// Check for existence of Torsion already
	if (hasTorsion(i, j, k, l))
	{
		Messenger::warn("Refused to add a new Torsion between atoms %i, %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_.get());
		return NULL;
	}

	// OK to add new torsion
	SpeciesTorsion* t = torsions_.add();
	t->setParent(this);
	t->setAtoms(i, j, k, l);

	return t;
}

// Add new torsion definition
SpeciesTorsion* Species::addTorsion(int i, int j, int k, int l)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}
	if ((l < 0) || (l >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'l' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", l, name_.get());
		return false;
	}

	return addTorsion(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}

// Return number of torsions in list
int Species::nTorsions() const
{
	return torsions_.nItems();
}

// Return list of torsions
SpeciesTorsion* Species::torsions() const
{
	return torsions_.first();
}

// Return nth torsion
SpeciesTorsion* Species::torsion(int n)
{
	return torsions_[n];
}

// Return whether torsion between atoms exists
bool Species::hasTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const
{
	for (SpeciesTorsion* t = torsions_.first(); t != NULL; t = t->next) if (t->matches(i, j, k, l)) return true;
	return false;
}

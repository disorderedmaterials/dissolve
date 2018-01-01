/*
	*** Grain
	*** src/classes/grain.cpp
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

#include "classes/grain.h"
#include "classes/speciesgrain.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"

// Constructor
Grain::Grain()
{
	molecule_ = NULL;
}

// Destructor
Grain::~Grain()
{
	clear();
}

// Clear all data
void Grain::clear()
{
	atoms_.clear();
}

/*
 * Atoms
 */

// Initialise
void Grain::initialise(int nAtoms)
{
	atoms_.createEmpty(nAtoms);
}

// Set Molecule that contains this grain
void Grain::setMolecule(Molecule* mol)
{
	molecule_ = mol;
}

// Return Molecule that contains this grain
Molecule* Grain::molecule() const
{
	return molecule_;
}

// Add Atom pointer pointer to list (also sets Grain pointer in Atom)
void Grain::addAtom(Atom* i)
{
#ifdef CHECKS
	// Check for NULL pointer
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer pointer passed to Grain::addAtom().\n");
		return;
	}
#endif
	atoms_.add(i);

	if (i->grain() != NULL) Messenger::warn("Grain pointer in Atom %i is already set, and we are about to overwrite it.\n", i->arrayIndex());
	i->setGrain(this);
}

// Return number of Atoms in Grain
int Grain::nAtoms() const
{
	return atoms_.nItems();
}

// Return atoms array
Atom** Grain::atoms()
{
	return atoms_.array();
}

// Return nth Atom in Grain
Atom* Grain::atom(int n) const
{
#ifdef CHECKS
	static Atom dummy;
	if ((n < 0) || (n >= nAtoms()))
	{
		Messenger::print("OUT_OF_RANGE - Atom index (%i) given to Grain::atom() is out of range (nAtoms = %i).\n", n, nAtoms());
		return NULL;
	}
	if (atoms_.value(n) == NULL)
	{
		Messenger::print("NULL_POINTER - Atom pointer pointer for index %i in Grain::atom() is NULL.\n", n);
		return &dummy;
	}
#endif
	return atoms_.value(n);
}

/*
 * Coordinates / Manipulation
 */

// Translate Grain centre
void Grain::translate(const Vec3<double>& delta)
{
	for (int n=0; n<nAtoms(); ++n) atom(n)->translateCoordinates(delta);
}

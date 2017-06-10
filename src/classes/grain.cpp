/*
	*** Grain
	*** src/classes/grain.cpp
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

#include "classes/grain.h"
#include "classes/speciesgrain.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"

// Constructor
Grain::Grain()
{
	atoms_ = NULL;
	nAtoms_ = 0;
	localIndex_ = -1;
	index_ = -1;
	parent_ = NULL;
	source_ = NULL;
}

// Destructor
Grain::~Grain()
{
	clear();
}

// Clear all data
void Grain::clear()
{
	if (atoms_ != NULL) delete[] atoms_;
	atoms_ = NULL;
	nAtoms_ = 0;
	atomsArraySize_ = 0;
}

/*
 * Atoms
 */

// Initialise
bool Grain::initialise(SpeciesGrain* sg)
{
	/*
	 * Initialise the Grain structure based on the GrainDefinition supplied. The resulting Grain will have enough
	 * storage to reference the number of Atoms contained in the original GrainDefinition.
	 */
	clear();
#ifdef CHECKS
	// Check for NULL pointer
	if (sg == NULL)
	{
		Messenger::error("NULL_POINTER - NULL GrainDefinition pointer passed to Grain::initialise().\n");
		return false;
	}
#endif
	source_ = sg;
	if (source_->nAtoms() < 1)
	{
		Messenger::print("BAD_ARRAY - Tried to initialise an array of size %i in Grain::initialise().\n", source_->nAtoms());
		return false;
	}
	atomsArraySize_ = source_->nAtoms();
	atoms_ = new Atom*[atomsArraySize_];
	nAtoms_ = 0;
	return true;
}

// Set parent Molecule
void Grain::setParent(Molecule* mol)
{
	parent_ = mol;
}

// Return parent Molecule
Molecule* Grain::parent() const
{
	return parent_;
}

// Return source SpeciesGrain
const SpeciesGrain* Grain::source() const
{
	return source_;
}

// Add atom pointer pointer to list
bool Grain::addAtom(Atom* i)
{
#ifdef CHECKS
	// Check for NULL pointer
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer pointer passed to Grain::addAtom().\n");
		return false;
	}
#endif
	if (nAtoms_ < atomsArraySize_) 
	{
		atoms_[nAtoms_] = i;
		atoms_[nAtoms_]->setGrain(this);

		++nAtoms_;
	}
	else
	{
		Messenger::error("Tried to add too many Atoms to Grain.\n");
		return false;
	}
	return true;
}

// Return number of Atoms in Grain
int Grain::nAtoms() const
{
	return nAtoms_;
}

// Return atoms array
Atom** Grain::atoms() const
{
	return atoms_;
}

// Return nth Atom in Grain
Atom* Grain::atom(int n) const
{
#ifdef CHECKS
	static Atom dummy;
	if ((n < 0) || (n >= nAtoms_))
	{
		Messenger::print("OUT_OF_RANGE - Atom index (%i) given to Grain::atom() is out of range (nAtoms_ = %i).\n", n, nAtoms_);
		return NULL;
	}
	if (atoms_[n] == NULL)
	{
		Messenger::print("NULL_POINTER - Atom pointer pointer for index %i in Grain::atom() is NULL.\n", n);
		return &dummy;
	}
#endif
	return atoms_[n];
}

// Set index of Grain
void Grain::setIndex(int index)
{
#ifdef CHECKS
	if (index_ != -1)
	{
		Messenger::print("BAD_USAGE - Tried to set the index of a Grain twice.\n");
		return;
	}
#endif
	index_ = index;
}

// Return index of Grain
int Grain::index() const
{
	return index_;
}

/*
 * Coordinates / Manipulation
 */

// Translate Grain centre
void Grain::translate(const Vec3<double>& delta)
{
	for (int n=0; n<nAtoms_; ++n) atom(n)->translateCoordinates(delta);
}

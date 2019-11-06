/*
	*** Molecule Definition
	*** src/classes/molecule.cpp
	Copyright T. Youngs 2012-2019

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

#include "classes/molecule.h"
#include "classes/angle.h"
#include "classes/atom.h"
#include "classes/bond.h"
#include "classes/grain.h"
#include "classes/torsion.h"
#include "classes/box.h"

// Constructor
Molecule::Molecule() : DynamicArrayObject<Molecule>()
{
	species_ = NULL;

	// Set sensible defaults for Arrays
	atoms_.setChunkSize(2);
	grains_.setChunkSize(2);
}

// Destructor
Molecule::~Molecule()
{
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void Molecule::clear()
{
	species_ = NULL;

	atoms_.clear();
	grains_.clear();
}

/*
 * Atoms / Grains
 */

// Set Species that this Molecule represents
void Molecule::setSpecies(const Species* sp)
{
	species_ = sp;
}

// Return Species that this Molecule represents
const Species* Molecule::species() const
{
	return species_;
}

// Add Atom to Molecule
void Molecule::addAtom(Atom* i)
{
	atoms_.add(i);

	if (i->molecule() != NULL) Messenger::warn("Molecule parent is already set in Atom id %i, and we are about to overwrite it...\n", i->arrayIndex());
	i->setMolecule(this);
}

// Return size of Atom array
int Molecule::nAtoms() const
{
	return atoms_.nItems();
}

// Return atoms array
Atom** Molecule::atoms()
{
	return atoms_.array();
}

// Return nth Atom pointer
Atom* Molecule::atom(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n >= nAtoms()))
	{
		Messenger::print("OUT_OF_RANGE - Atom index %i is out of range in Molecule::atom().\n", n);
		return NULL;
	}
	if (atoms_.constAt(n) == NULL)
	{
		Messenger::print("NULL_POINTER - The pointer to Atom %i in Molecule::atom() is NULL.\n", n);
		return NULL;
	}
#endif
	return atoms_.constAt(n);
}

// Add Grain to Molecule
void Molecule::addGrain(Grain* grain)
{
	grains_.add(grain);

	if (grain->molecule() != NULL) Messenger::warn("Molecule parent is already set in Grain id %i, and we are about to overwrite it...\n", grain->arrayIndex());
	grain->setMolecule(this);
}

// Return size of Grain array
int Molecule::nGrains() const
{
	return grains_.nItems();
}

// Return nth Grain pointer
Grain* Molecule::grain(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= grains_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Grain index %i is out of range in Molecule::grain() (nGrains = %i).\n", n, grains_.nItems());
		return NULL;
	}
#endif
	return grains_[n];
}

/*
 * Manipulations
 */

// Set centre of geometry of molecule
void Molecule::setCentreOfGeometry(const Box* box, const Vec3<double> newCentre)
{
	// Calculate Molecule centre of geometry
	Vec3<double> newR, cog = centreOfGeometry(box);

	// Apply transform
	for (int n=0; n<nAtoms(); ++n)
	{
		newR = box->minimumVector(atom(n), cog) + newCentre;
		atom(n)->setCoordinates(newR);
	}
}

// Calculate and return centre of geometry
Vec3<double> Molecule::centreOfGeometry(const Box* box) const
{
	if (nAtoms() == 0) return Vec3<double>();
	
	// Calculate center relative to first atom in molecule
	Vec3<double> cog = atom(0)->r();
	for (int n = 1; n<nAtoms(); ++n) cog += box->minimumImage(atom(n), atom(0)->r());

	return (cog / nAtoms());
}

// Transform molecule with supplied matrix, using centre of geometry as the origin
void Molecule::transform(const Box* box, const Matrix3& transformationMatrix)
{
	// Calculate Molecule centre of geometry
	Vec3<double> newR, cog = centreOfGeometry(box);
	
	// Apply transform
	for (int n=0; n<nAtoms(); ++n)
	{
		newR = transformationMatrix * box->minimumVector(cog, atom(n)->r()) + cog;
		atom(n)->setCoordinates(newR);
	}
}

// Transform selected atoms with supplied matrix, around specified origin
void Molecule::transform(const Box* box, const Matrix3& transformationMatrix, const Vec3<double>& origin, int nTargetAtoms, int* targetAtoms)
{
	// Loop over supplied Atoms
	Vec3<double> newR;
	Atom* i;
	for (int n=0; n<nTargetAtoms; ++n)
	{
		i = atom(targetAtoms[n]);
		newR = transformationMatrix * box->minimumVector(origin, i->r()) + origin;
		i->setCoordinates(newR);
	}
}

// Translate whole molecule by the delta specified
void Molecule::translate(const Vec3<double> delta)
{
	for (int n=0; n<nAtoms(); ++n) atom(n)->translateCoordinates(delta);
}

// Translate specified atoms by the delta specified
void Molecule::translate(const Vec3<double>& delta, int nTargetAtoms, int* targetAtoms)
{
	for (int n=0; n<nTargetAtoms; ++n) atom(targetAtoms[n])->translateCoordinates(delta);
}

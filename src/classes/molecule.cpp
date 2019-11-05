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
	bonds_.setChunkSize(2);
	angles_.setChunkSize(2);
	torsions_.setChunkSize(2);
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
	bonds_.clear();
	angles_.clear();
	torsions_.clear();
}

/*
 * Atoms / Grains
 */

// Set Species which the Molecule represents
void Molecule::setSpecies(Species* sp)
{
	species_ = sp;
}

// Return Species which the Molecule represents
Species* Molecule::species() const
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

// Add Bond to Molecule
void Molecule::addBond(Bond* bond)
{
	bonds_.add(bond);
}

// Return size of Bond array
int Molecule::nBonds() const
{
	return bonds_.nItems();
}

// Return Bond array
Bond** Molecule::bonds()
{
	return bonds_;
}

// Return nth Bond pointer
Bond* Molecule::bond(int n) const
{
	return bonds_.constAt(n);
}

// Add Angle to Molecule
void Molecule::addAngle(Angle* angle)
{
	angles_.add(angle);
}

// Return size of Angle array
int Molecule::nAngles() const
{
	return angles_.nItems();
}

// Return Angle array
Angle** Molecule::angles()
{
	return angles_;
}

// Return nth Angle pointer
Angle* Molecule::angle(int n) const
{
	return angles_.constAt(n);
}

// Add Torsion to Molecule
void Molecule::addTorsion(Torsion* torsion)
{
	torsions_.add(torsion);
}

// Return size of Torsion array
int Molecule::nTorsions() const
{
	return torsions_.nItems();
}

// Return Torsion array
Torsion** Molecule::torsions()
{
	return torsions_;
}

// Return nth Torsion pointer
Torsion* Molecule::torsion(int n) const
{
	return torsions_.constAt(n);
}

/*
 * Upkeep
 */

// Select Atoms along any path from the specified one
void Molecule::selectFromAtom(Atom* i, RefList<Atom>& selectedAtoms, Bond* excludedBond1, Bond* excludedBond2)
{
	// Add this Atom to our list
	selectedAtoms.addUnique(i);

	// Loop over Bonds on this Atom
	Atom* j;
	for (int n=0; n<i->bonds().nItems(); ++n)
	{
		Bond* bond = i->bonds().value(n);

		// Is this an excluded Bond?
		if (excludedBond1 == bond) continue;
		if (excludedBond2 == bond) continue;

		// Get Bond partner Atom and begin selection from it unless it's already in the list
		j = bond->partner(i);
		if (selectedAtoms.contains(j)) continue;
		selectFromAtom(j, selectedAtoms, excludedBond1, excludedBond2);
	}
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

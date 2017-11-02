/*
	*** Molecule Definition
	*** src/classes/molecule.cpp
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
	// Set sensible defaults for Arrays
	atoms_.setChunkSize(8);
	grains_.setChunkSize(8);
	bonds_.setChunkSize(8);
	angles_.setChunkSize(8);
	torsions_.setChunkSize(8);
}

// Destructor
Molecule::~Molecule()
{
}

/*
 * Atoms / Grains
 */

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
	if (atoms_.value(n) == NULL)
	{
		Messenger::print("NULL_POINTER - The pointer to Atom %i in Molecule::atom() is NULL.\n", n);
		return NULL;
	}
#endif
	return atoms_.value(n);
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

	if (bond->molecule() != NULL) Messenger::warn("Molecule parent is already set in Bond id %i, and we are about to overwrite it...\n", bond->arrayIndex());
	bond->setMolecule(this);
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
	return bonds_.value(n);
}

// Add Angle to Molecule
void Molecule::addAngle(Angle* angle)
{
	angles_.add(angle);

	if (angle->molecule() != NULL) Messenger::warn("Molecule parent is already set in Angle id %i, and we are about to overwrite it...\n", angle->arrayIndex());
	angle->setMolecule(this);
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
	return angles_.value(n);
}

// Add Torsion to Molecule
void Molecule::addTorsion(Torsion* torsion)
{
	torsions_.add(torsion);

	if (torsion->molecule() != NULL) Messenger::warn("Molecule parent is already set in Torsionid %i, and we are about to overwrite it...\n", torsion->arrayIndex());
	torsion->setMolecule(this);
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
	return torsions_.value(n);
}

/*
 * Manipulations
 */

// Calculate and return centre of geometry
Vec3<double> Molecule::centreOfGeometry(const Box* box) const
{
	if (nAtoms() == 0) return Vec3<double>();
	
	// Calculate center relative to first atom in molecule
	Vec3<double> cog = atom(0)->r();
	for (int n = 1; n<nAtoms(); ++n) cog += box->minimumImage(atom(n), atom(0)->r());

	return (cog / nAtoms());
}

// Transform molecule with supplied matrix
void Molecule::applyTransform(const Box* box, const Matrix3& transform)
{
	// Calculate Molecule centre of geometry
	Vec3<double> newR, cog = centreOfGeometry(box);
	
	// Apply transform
	for (int n=0; n<nAtoms(); ++n)
	{
		newR = transform * box->minimumVector(cog, atom(n)->r()) + cog;
		atom(n)->setCoordinates(newR);
	}
}

// Transform selected atoms with supplied matrix
void Molecule::applyTransform(const Box* box, const Matrix3& transform, const Vec3<double>& origin, int nTargetAtoms, Atom** targetAtoms)
{
	// Loop over supplied Atoms
	Vec3<double> newR;
	Atom* i;
	for (int n=0; n<nTargetAtoms; ++n)
	{
		i = targetAtoms[n];
		newR = transform * (i->r() - origin) + origin;
		i->setCoordinates(newR);
	}
}

// Set centre of geometry of molecule
void Molecule::setCentre(const Box* box, const Vec3<double> newCentre)
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

// Translate centre of geometry
void Molecule::translateCentre(const Vec3<double> delta)
{
	for (int n=0; n<nAtoms(); ++n) atom(n)->translateCoordinates(delta);
}

// Randomise geometry
void Molecule::randomiseGeometry(const Box* box)
{
	/*
	 * Randomise the geometry of the Molecule, exploring conformational space as best we can. This routine should be used when
	 * instantiating the initial ensemble of Molecules such that a variety of conformations exist in the starting system. Each defined
	 * Bond is taken in turn, and the connected Atoms at one end (chosen randomly) are rotated about the Bond axis.
	 */ 

	// Use Bond definitions in parent Species
	int terminus;
	Matrix3 transform;
	Vec3<double> axis;
	Atom* localI, *localJ;
	for (int n=0; n<bonds_.nItems(); ++n)
	{
		// Get pointer to Bond
		Bond* b = bonds_[n];

		// Select random terminus
		terminus = DUQMath::random() > 0.5;
		if (b->nAttached(terminus) < 2) continue;

		// Get local Atom pointers
#ifdef CHECKS
		if (b->i() == NULL)
		{
			Messenger::error("NULL_POINTER - NULL Atom pointer 'i' found in Molecule::randomiseGeometry().\n");
			return;
		}
		if (b->j() == NULL)
		{
			Messenger::error("NULL_POINTER - NULL Atom pointer 'j' found in Molecule::randomiseGeometry().\n");
			return;
		}
#endif
		localI = b->i();
		localJ = b->j();

		// Create axis rotation matrix
		axis = localI->r() - localJ->r();
		transform.createRotationAxis(axis.x, axis.y, axis.z, DUQMath::random()*360.0, true);

		// Perform transform
		applyTransform(box, transform, terminus == 0 ? localI->r() : localJ->r(), b->nAttached(terminus), b->attached(terminus));
	}
}

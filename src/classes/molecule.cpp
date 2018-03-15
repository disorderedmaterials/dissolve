/*
	*** Molecule Definition
	*** src/classes/molecule.cpp
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
	atoms_.clear();
	grains_.clear();
	bonds_.clear();
	angles_.clear();
	torsions_.clear();
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

	if (torsion->molecule() != NULL) Messenger::warn("Molecule parent is already set in Torsion id %i, and we are about to overwrite it...\n", torsion->arrayIndex());
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
 * Upkeep
 */

// Select Atoms along any path from the specified one
void Molecule::selectFromAtom(Atom* i, RefList<Atom,bool>& selectedAtoms, Bond* excludedBond1, Bond* excludedBond2)
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

// Recalculate attached Atom lists for all intramolecular terms involved in the Molecule
void Molecule::updateAttachedAtomLists()
{
	RefList<Atom,bool> selectedAtoms;

	// Bonds
	for (int n=0; n<bonds_.nItems(); ++n)
	{
		// Grab Bond pointer
		Bond* b = bonds_[n];

		// Select all Atoms attached to Atom 'i', excluding the Bond as a path
		selectedAtoms.clear();
		selectFromAtom(b->i(), selectedAtoms, b);

		// If the list now contains Atom j, the two atoms are present in a cycle of some sort, and we can only add the Atom 'i' itself
		// In that case we can also finish the list for Atom 'j', and continue the loop.
		if (selectedAtoms.contains(b->j()))
		{
			Messenger::printVerbose("Bond between Atoms %i-%i is present in a cycle, so a minimal set of attached atoms will be used.\n", b->i()->arrayIndex(), b->j()->arrayIndex());
			b->setAttachedAtoms(0, b->i());
			b->setAttachedAtoms(1, b->j());
			continue;
		}
		else b->setAttachedAtoms(0, selectedAtoms);

		// Select all Atoms attached to Atom 'i', excluding the Bond as a path
		selectedAtoms.clear();
		selectFromAtom(b->j(), selectedAtoms, b);
		b->setAttachedAtoms(1, selectedAtoms);
	}

	// Angles - termini are 'i' and 'k'
	for (int n=0; n<angles_.nItems(); ++n)
	{
		// Grab Angle pointer
		Angle* a = angles_[n];

		// Grab relevant Bonds (if they exist)
		Bond* ji = a->j()->findBond(a->i());
		Bond* jk = a->j()->findBond(a->k());

		// Select all Atoms attached to Atom 'i', excluding the Bond ji as a path
		selectedAtoms.clear();
		selectFromAtom(a->i(), selectedAtoms, ji, jk);

		// Remove Atom 'j' from the list if it's there
		selectedAtoms.remove(a->j());

		// If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the Atom 'i' itself
		// In that case we can also finish the list for Atom 'k', and continue the loop.
		if (selectedAtoms.contains(a->k()))
		{
			Messenger::printVerbose("Angle between Atoms %i-%i-%i is present in a cycle, so a minimal set of attached atoms will be used.\n", a->i()->arrayIndex(), a->j()->arrayIndex(), a->k()->arrayIndex());
			a->setAttachedAtoms(0, a->i());
			a->setAttachedAtoms(1, a->k());
			continue;
		}
		else a->setAttachedAtoms(0, selectedAtoms);

		// Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
		selectedAtoms.clear();
		selectFromAtom(a->k(), selectedAtoms, ji, jk);

		// Remove Atom 'j' from the list if it's there
		selectedAtoms.remove(a->j());

		a->setAttachedAtoms(1, selectedAtoms);
	}

	// Torsions - termini are 'j' and 'k'
	for (int n=0; n<torsions_.nItems(); ++n)
	{
		// Grab Torsion pointer
		Torsion* t = torsions_[n];

		// Grab relevant Bond (if it exists)
		Bond* jk = t->j()->findBond(t->k());

		// Select all Atoms attached to Atom 'j', excluding the Bond ji as a path
		selectedAtoms.clear();
		selectFromAtom(t->j(), selectedAtoms, jk);

		// Remove Atom 'j' from the list
		selectedAtoms.remove(t->j());

		// If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the Atom 'i'
		if (selectedAtoms.contains(t->k()))
		{
			Messenger::printVerbose("Torsion between Atoms %i-%i-%i-%i is present in a cycle, so a minimal set of attached atoms will be used.\n", t->i()->arrayIndex(), t->j()->arrayIndex(), t->k()->arrayIndex(), t->l()->arrayIndex());
			t->setAttachedAtoms(0, t->i());
			t->setAttachedAtoms(1, t->l());
			continue;
		}
		else t->setAttachedAtoms(0, selectedAtoms);

		// Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
		selectedAtoms.clear();
		selectFromAtom(t->k(), selectedAtoms, jk);

		// Remove Atom 'k' from the list
		selectedAtoms.remove(t->k());

		t->setAttachedAtoms(1, selectedAtoms);
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
void Molecule::transform(const Box* box, const Matrix3& transformationMatrix, const Vec3<double>& origin, int nTargetAtoms, Atom** targetAtoms)
{
	// Loop over supplied Atoms
	Vec3<double> newR;
	Atom* i;
	for (int n=0; n<nTargetAtoms; ++n)
	{
		i = targetAtoms[n];
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
void Molecule::translate(const Vec3<double>& delta, int nTargetAtoms, Atom** targetAtoms)
{
	for (int n=0; n<nTargetAtoms; ++n) targetAtoms[n]->translateCoordinates(delta);
}

// Randomise conformation by rotating around bond terms
void Molecule::randomiseConformation(const Box* box)
{
	/*
	 * Randomise the geometry of the Molecule, exploring conformational space as best we can. This routine should be used when
	 * instantiating the initial ensemble of Molecules such that a variety of conformations exist in the starting system. Each defined
	 * Bond is taken in turn, and the connected Atoms at one end (chosen randomly) are rotated about the Bond axis.
	 */ 

	// Use Bond definitions in parent Species
	int terminus;
	Matrix3 transformationMatrix;
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
		transformationMatrix.createRotationAxis(axis.x, axis.y, axis.z, DUQMath::random()*360.0, true);

		// Perform transform
		transform(box, transformationMatrix, terminus == 0 ? localI->r() : localJ->r(), b->nAttached(terminus), b->attached(terminus));
	}
}

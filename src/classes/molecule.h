/*
	*** Molecule
	*** src/classes/molecule.h
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

#ifndef DUQ_MOLECULE_H
#define DUQ_MOLECULE_H

#include "templates/array.h"
#include "templates/dynamicarrayobject.h"
#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class Angle;
class Atom;
class Bond;
class Box;
class Grain;
class Species;
class SpeciesAtom;
class SpeciesGrain;
class Matrix3;
class Torsion;

// Molecule Definition
class Molecule : public DynamicArrayObject<Molecule>
{
	/*
	 * A Molecule can be thought of as a collection of Atoms / Grains that belong together, and can be moved as such.
	 * A Molecule does not itself own its Atoms or Grains. Instead, pointers to the relevant Atoms/Grains in the parent
	 * Configuration are stored.
	 */
	public:
	// Constructor
	Molecule();
	// Destructor
	~Molecule();


	/*
	 * Atoms / Grains
	 */
	private:
	// Array of pointers to Atoms
	Array<Atom*> atoms_;
	// Array of Grain pointers
	Array<Grain*> grains_;
	// Array of Bonds this Molecule contains
	Array<Bond*> bonds_;
	// Array of Angles this Molecule contains
	Array<Angle*> angles_;
	// Array of Torsions this Molecule contains
	Array<Torsion*> torsions_;

	public:
	// Add Atom to Molecule
	void addAtom(Atom* i);
	// Return size of Atom array
	int nAtoms() const;
	// Return Atoms array
	Atom** atoms();
	// Return nth Atom pointer
	Atom* atom(int n) const;
	// Add Grain to Molecule
	void addGrain(Grain* grain);
	// Return size of Grain array
	int nGrains() const;
	// Return nth Grain pointer
	Grain* grain(int n);
	// Add Bond to Molecule
	void addBond(Bond* bond);
	// Return size of Bond array
	int nBonds() const;
	// Return Bonds array
	Bond** bonds();
	// Return nth Bond pointer
	Bond* bond(int n);
	// Add Angle to Molecule
	void addAngle(Angle* angle);
	// Return size of Angle array
	int nAngles() const;
	// Return Angles array
	Angle** angles();
	// Return nth Angle pointer
	Angle* angle(int n);
	// Add Torsion to Molecule
	void addTorsion(Torsion* torsion);
	// Return size of Torsion array
	int nTorsions() const;
	// Return Torsions array
	Torsion** torsions();
	// Return nth Torsion pointer
	Torsion* torsion(int n);


	/*
	 * Manipulations
	 */
	public:
	// Calculate and return centre of geometry
	Vec3<double> centreOfGeometry(const Box* box) const;
	// Transform molecule with supplied matrix and translation vector
	void applyTransform(const Box* box, const Matrix3& transform);
	// Transform selected Atoms
	void applyTransform(const Box* box, const Matrix3& transform, const Vec3<double>& origin, int nTargetAtoms, Atom** targetAtoms);
	// Set centre of geometry
	void setCentre(const Box* box, const Vec3<double> newCentre);
	// Translate centre of geometry
	void translateCentre(const Vec3<double> delta);
	// Randomise geometry utilising stored Bond terms
	void randomiseGeometry(const Box* box);
};

#endif

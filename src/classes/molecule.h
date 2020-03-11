/*
	*** Molecule
	*** src/classes/molecule.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_MOLECULE_H
#define DISSOLVE_MOLECULE_H

#include <vector>
#include "templates/dynamicarrayobject.h"
#include "templates/vector3.h"

// Forward Declarations
class Atom;
class Box;
class Matrix3;
class Species;

// Molecule Definition
class Molecule : public DynamicArrayObject<Molecule>
{
	public:
	// Constructor
	Molecule();
	// Destructor
	~Molecule();


	/*
	 * DynamicArrayObject Virtuals
	 */
	public:
	// Clear object, ready for re-use
	void clear();


	/*
	 * Contents
	 */
	private:
	// Species that this Molecule represents
	const Species* species_;
	// Array of pointers to Atoms that belong to this Molecule (stored in Configuration)
	std::vector<Atom*> atoms_;

	public:
	// Set Species that this Molecule represents
	void setSpecies(const Species* sp);
	// Return Species that this Molecule represents
	const Species* species() const;
	// Add Atom to Molecule
	void addAtom(Atom* i);
	// Return size of Atom array
	int nAtoms() const;
	// Return Atoms array
	std::vector<Atom*>& atoms();
	// Return nth Atom pointer
	Atom* atom(int n) const;


	/*
	 * Manipulations
	 */
	public:
	// Set centre of geometry
	void setCentreOfGeometry(const Box* box, const Vec3<double> newCentre);
	// Calculate and return centre of geometry
	Vec3<double> centreOfGeometry(const Box* box) const;
	// Transform molecule with supplied matrix, using centre of geometry as the origin
	void transform(const Box* box, const Matrix3& transformationMatrix);
	// Transform selected atoms with supplied matrix, around specified origin
	void transform(const Box* box, const Matrix3& transformationMatrix, const Vec3< double >& origin, int nTargetAtoms, int* targetAtoms);
	// Translate whole molecule by the delta specified
	void translate(const Vec3<double> delta);
	// Translate specified atoms by the delta specified
	void translate(const Vec3<double>& delta, int nTargetAtoms, int* targetAtoms);
};

#endif

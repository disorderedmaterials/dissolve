/*
	*** Grain
	*** src/classes/grain.h
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

#ifndef DISSOLVE_GRAIN_H
#define DISSOLVE_GRAIN_H

#include "math/matrix3.h"
#include "templates/array.h"
#include "templates/dynamicarrayobject.h"

// Forward Declarations
class Atom;
class Molecule;

/*
 * Grain
 */
class Grain : public DynamicArrayObject<Grain>
{
	public:
	// Constructor
	Grain();
	// Destructor
	~Grain();
	// Clear all data
	void clear();


	/*
	 * Atoms / Character
	 */
	private:
	// Parent molecule
	Molecule* molecule_;
	// Array of Atoms in this Grain
	Array<Atom*> atoms_;

	public:
	// Initialise
	void initialise(int nAtoms);
	// Set Molecule that contains this Grain
	void setMolecule(Molecule* mol);
	// Return Molecule that contains this Grain
	Molecule* molecule() const;
	// Add Atom pointer to list
	void addAtom(Atom* i);
	// Return number of Atoms in Grain
	int nAtoms() const;
	// Return Atoms array
	Atom** atoms();
	// Return nth Atom in Grain
	Atom* atom(int n) const;


	/*
	 * Coordinates / Manipulation
	 */
	public:
	// Translate grain centre
	void translate(const Vec3<double>& delta);
};

#endif

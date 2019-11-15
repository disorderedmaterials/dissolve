/*
	*** Species Ring Definition
	*** src/classes/speciesring.h
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

#ifndef DISSOLVE_SPECIESRING_H
#define DISSOLVE_SPECIESRING_H

#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/orderedpointerdataarray.h"
#include "templates/pointerarray.h"
#include "templates/vector3.h"
#include "templates/reflist.h"

// Forward Declarations
class SpeciesAtom;

// SpeciesRing Definition
class SpeciesRing : public ListItem<SpeciesRing>
{
	public:
	// Constructor
	SpeciesRing();
	// Destructor
	~SpeciesRing();


	/*
	 * Atoms
	 */
	private:
	// Array of atoms in the ring, in the order in which they appear
	PointerArray<const SpeciesAtom> atoms_;

	public:
	// Set atoms in ring
	void setAtoms(const PointerArray<const SpeciesAtom>& atoms);
	// Return array of atoms in ring
	const PointerArray<const SpeciesAtom>& atoms() const;
	// Return size of ring (number of atoms in array)
	int size() const;
	// Print ring information
	void print() const;


	/*
	 * Comparison
	 */
	public:
	// Equality operator
	bool operator==(const SpeciesRing& other);
};

#endif

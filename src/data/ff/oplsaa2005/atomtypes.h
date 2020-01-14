/*
	*** OPLSAA (2005) Atom Types
	*** src/data/ff/oplsaa2005/atomtypes.h
	Copyright T. Youngs 2019-2020

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

#ifndef DISSOLVE_FORCEFIELD_OPLS2005_ATOMTYPES_H
#define DISSOLVE_FORCEFIELD_OPLS2005_ATOMTYPES_H

#include "data/ff.h"
#include "templates/array.h"

// Forward Declarations
class CoreData;
class SpeciesAtom;

// OPLSAA (2005) Atom Types Forcefield
class OPLSAA2005_AtomTypes
{
	/*
	 * Search Functions
	 */
	public:
	// Return atom type information with index specified
	static const ForcefieldAtomType& atomTypeByIndex(int id);


	/*
	 * Data
	 */
	public:
	// Return formatted publication references
	static const char* publicationReferences();

	private:
	// Return number of atom type definitions available
	static int nAtomTypes();
	// Return atom type definitions
	static const ForcefieldAtomType* atomTypes();
};

#endif

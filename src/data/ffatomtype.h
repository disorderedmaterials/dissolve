/*
	*** Forcefield Atom Typw
	*** src/data/ffatomtype.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_FORCEFIELDATOMTYPE_H
#define DISSOLVE_FORCEFIELDATOMTYPE_H

#include "base/charstring.h"

// Forward Declarations
class Forcefield;

// Forcefield AtomType Base Class
class ForcefieldAtomType
{
	public:
	// Constructor / Destructor
	ForcefieldAtomType(int index, const char* name, const char* description);
	virtual ~ForcefieldAtomType();


	/*
	 * Identity
	 */
	private:
	// Index of atom type
	int index_;
	// Name of atom type
	CharString typeName_;
	// Brief description of tyoe
	CharString typeDescription_;

	public:
	// Return index of type
	int index() const;
	// Return name of type
	const char* typeName() const;
	// Return description for type
	const char* typeDescription() const;
};

#endif

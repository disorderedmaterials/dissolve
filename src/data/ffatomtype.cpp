/*
	*** ForcefieldAtomType Atom Type
	*** src/data/ffatomtype.cpp
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

#include "data/ffatomtype.h"

// Constructor / Destructor
ForcefieldAtomType::ForcefieldAtomType(int index, const char* name, const char* description)
{
	index_ = index;
	typeName_ = name;
	typeDescription_ = description;
}

ForcefieldAtomType::~ForcefieldAtomType()
{
}

/*
 * Identity
 */

// Return index of type
int ForcefieldAtomType::index() const
{
	return index_;
}

// Return name of type
const char* ForcefieldAtomType::typeName() const
{
	return typeName_.get();
}

// Return description for type
const char* ForcefieldAtomType::typeDescription() const
{
	return typeDescription_.get();
}

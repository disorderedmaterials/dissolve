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
#include "data/ff.h"

// Constructors
ForcefieldAtomType::ForcefieldAtomType(Forcefield* parent, int z, int index, const char* name, const char* description, double q, double data0, double data1, double data2, double data3) : ElementReference(z), ListItem<ForcefieldAtomType>()
{
	index_ = index;
	name_ = name;
	description_ = description;
	parameters_.setCharge(q);
	parameters_.setParameter(0, data0);
	parameters_.setParameter(1, data1);
	parameters_.setParameter(2, data2);
	parameters_.setParameter(3, data3);

	// Register this atom type with the parent forcefield
	if (parent) parent->registerAtomType(this, z);
}
ForcefieldAtomType::ForcefieldAtomType(Forcefield* parent, const char* sanityName, const ForcefieldAtomType& sourceType) : ElementReference(sourceType.Z()), ListItem<ForcefieldAtomType>()
{
	// Copy data from the supplied source
	index_ = sourceType.index_;
	name_ = sourceType.name_;
	if (!DissolveSys::sameString(name_, sanityName)) Messenger::warn("Sanity typename '%s' differs from the supplied data ('%s').\n", sanityName, sourceType.name());
	description_ = sourceType.description_;
	parameters_ = sourceType.parameters_;

	// Register this atom type with the parent forcefield
	if (parent) parent->registerAtomType(this, sourceType.Z());
}

// Destructor
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
const char* ForcefieldAtomType::name() const
{
	return name_.get();
}

// Return description for type
const char* ForcefieldAtomType::description() const
{
	return description_.get();
}

/*
 * Parameters
 */

// Return short-range parameters
const Parameters& ForcefieldAtomType::parameters() const
{
	return parameters_;
}

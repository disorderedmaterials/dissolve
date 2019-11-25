/*
	*** ForcefieldParameters Atom Type
	*** src/data/ffparameters.cpp
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

#include "data/ffparameters.h"
#include "data/ff.h"

// Constructors
ForcefieldParameters::ForcefieldParameters(Forcefield* parent, const char* name, double data0, double data1, double data2, double data3)
{
	forcefield_ = parent;

	name_ = name;
	parameters_.setParameter(0, data0);
	parameters_.setParameter(1, data1);
	parameters_.setParameter(2, data2);
	parameters_.setParameter(3, data3);

	// Register this atom type with the parent forcefield
	if (parent) parent->registerParameters(this);
}

// Destructor
ForcefieldParameters::~ForcefieldParameters()
{
}

/*
 * Identity
 */

// Return parent Forcefield
const Forcefield* ForcefieldParameters::forcefield() const
{
	return forcefield_;
}

// Return name of type
const char* ForcefieldParameters::name() const
{
	return name_.get();
}

/*
 * Parameters
 */

// Return short-range parameters
const Parameters& ForcefieldParameters::parameters() const
{
	return parameters_;
}

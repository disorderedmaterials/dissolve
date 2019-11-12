/*
	*** Forcefield Bond Term
	*** src/data/ffbondterm.cpp
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

#include "data/ffbondterm.h"
#include "data/ffatomtype.h"
#include "data/ff.h"

// Constructor
ForcefieldBondTerm::ForcefieldBondTerm(Forcefield* parent, const char* typeI, const char* typeJ, SpeciesBond::BondFunction form, double data0, double data1, double data2, double data3)
{
	typeI_ = typeI;
	typeJ_ = typeJ;
	form_ = form;
	parameters_[0] = data0;
	parameters_[1] = data1;
	parameters_[2] = data2;
	parameters_[3] = data3;

	// Register this atom type with the parent forcefield
	if (parent) parent->registerBondTerm(this);
}

// Destructor
ForcefieldBondTerm::~ForcefieldBondTerm()
{
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldBondTerm::matches(const ForcefieldAtomType* i, const ForcefieldAtomType* j)
{
	if ((typeI_ == i->name()) && (typeJ_ == j->name())) return true;
	if ((typeJ_ == i->name()) && (typeI_ == j->name())) return true;

	return false;
}

// Return functional form index of interaction
SpeciesBond::BondFunction ForcefieldBondTerm::form() const
{
	return form_;
}

// Return array of parameters
const double* ForcefieldBondTerm::parameters() const
{
	return parameters_;
}

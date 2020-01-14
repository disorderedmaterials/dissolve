/*
	*** Forcefield Improper Term
	*** src/data/ffimproperterm.cpp
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

#include "data/ffimproperterm.h"
#include "data/ffatomtype.h"
#include "data/ff.h"

// Constructor
ForcefieldImproperTerm::ForcefieldImproperTerm(Forcefield* parent, const char* typeI, const char* typeJ, const char* typeK, const char* typeL, SpeciesImproper::ImproperFunction form, double data0, double data1, double data2, double data3)
{
	typeI_ = typeI;
	typeJ_ = typeJ;
	typeK_ = typeK;
	typeL_ = typeL;
	form_ = form;
	parameters_[0] = data0;
	parameters_[1] = data1;
	parameters_[2] = data2;
	parameters_[3] = data3;

	// Register this atom type with the parent forcefield
	if (parent) parent->registerImproperTerm(this);
}

// Destructor
ForcefieldImproperTerm::~ForcefieldImproperTerm()
{
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldImproperTerm::matches(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l)
{
	if (DissolveSys::sameWildString(typeI_, i->equivalentName()) && DissolveSys::sameWildString(typeJ_, j->equivalentName()) && DissolveSys::sameWildString(typeK_, k->equivalentName()) && DissolveSys::sameWildString(typeL_, l->equivalentName())) return true;
	if (DissolveSys::sameWildString(typeL_, i->equivalentName()) && DissolveSys::sameWildString(typeK_, j->equivalentName()) && DissolveSys::sameWildString(typeJ_, k->equivalentName()) && DissolveSys::sameWildString(typeI_, l->equivalentName())) return true;

	return false;
}

// Return functional form index of interaction
SpeciesImproper::ImproperFunction ForcefieldImproperTerm::form() const
{
	return form_;
}

// Return array of parameters
const double* ForcefieldImproperTerm::parameters() const
{
	return parameters_;
}

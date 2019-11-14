/*
	*** Forcefield Bond Term
	*** src/data/ffbondterm.h
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

#ifndef DISSOLVE_FORCEFIELD_BONDTERM_H
#define DISSOLVE_FORCEFIELD_BONDTERM_H

#include "classes/speciesbond.h"
#include "base/charstring.h"
#include "base/parameters.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Bond Term
class ForcefieldBondTerm : public ListItem<ForcefieldBondTerm>
{
	public:
	// Constructor / Destructor
	ForcefieldBondTerm(Forcefield* parent = NULL, const char* typeI = NULL, const char* typeJ = NULL, SpeciesBond::BondFunction form = SpeciesBond::nBondFunctions, double data0 = 0.0, double data1 = 0.0, double data2 = 0.0, double data3 = 0.0);
	~ForcefieldBondTerm();


	/*
	 * Data
	 */
	private:
	// Type names involved in interaction
	CharString typeI_, typeJ_;
	// Functional form of interaction
	SpeciesBond::BondFunction form_;
	// Parameters for interaction
	double parameters_[MAXINTRAPARAMS];

	public:
	// Return if this term matches the atom types supplied
	bool matches(const ForcefieldAtomType* i, const ForcefieldAtomType* j);
	// Return functional form index of interaction
	SpeciesBond::BondFunction form() const;
	// Return array of parameters
	const double* parameters() const;
};

#endif

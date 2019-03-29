/*
	*** SPC/Fw Forcefield
	*** src/data/ff/spcfw.h
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

#ifndef DISSOLVE_FORCEFIELD_SPCFW_H
#define DISSOLVE_FORCEFIELD_SPCFW_H

#include "data/ff.h"
#include "templates/array.h"

// Forward Declarations
class CoreData;
class Species;
class SpeciesAngle;
class SpeciesAtom;
class SpeciesBond;
class SpeciesIntra;
class SpeciesTorsion;

// SPC/Fw Forcefield
class Forcefield_SPCFw : public Forcefield
{
	public:
	// Constructor / Destructor
	Forcefield_SPCFw();
	~Forcefield_SPCFw();


	/*
	 * Definition
	 */
	public:
	// Return name of Forcefield
	const char* name();


	/*
	 * Atom Type Data
	 */
	public:
	// Determine and return atom type for specified SpeciesAtom
	ForcefieldAtomType* determineAtomType(SpeciesAtom* i) const;


	/*
	 * Term Generation
	 */
	private:
	// Generate bond parameters for the supplied UFF atom types
	bool generateBondTerm(const Species* sp, SpeciesBond* bondTerm, ForcefieldAtomType* i, ForcefieldAtomType* j) const;
	// Generate angle parameters for the supplied UFF atom types
	bool generateAngleTerm(const Species* sp, SpeciesAngle* angleTerm, ForcefieldAtomType* i, ForcefieldAtomType* j, ForcefieldAtomType* k) const;
	// Generate torsion parameters for the supplied UFF atom types
	bool generateTorsionTerm(const Species* sp, SpeciesTorsion* torsionTerm, ForcefieldAtomType* i, ForcefieldAtomType* j, ForcefieldAtomType* k, ForcefieldAtomType* l) const;

	public:
	// Assign suitable atom types to the supplied Species
	bool assignAtomTypes(Species* sp, CoreData& coreData, bool keepExisting = false) const;
};

#endif

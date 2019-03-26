/*
	*** Universal Forcefield
	*** src/data/ff/uff.h
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

#ifndef DISSOLVE_FORCEFIELD_UFF_H
#define DISSOLVE_FORCEFIELD_UFF_H

#include "data/ff.h"
#include "data/ff/uffatomtype.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class CoreData;
class Species;
class SpeciesAngle;
class SpeciesAtom;
class SpeciesBond;
class SpeciesIntra;
class SpeciesTorsion;

// Universal Forcefield
class Forcefield_UFF : public Forcefield
{
	public:
	// Constructor / Destructor
	Forcefield_UFF();
	~Forcefield_UFF();


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
	// Determine and return AtomType for specified SpeciesAtom
	UFFAtomType* determineAtomType(SpeciesAtom* i) const;


	/*
	 * Term Generation
	 */
	private:
	// Generate bond parameters for the supplied UFF atom types
	bool generateBondTerm(const Species* sp, SpeciesBond* bondTerm, UFFAtomType* i, UFFAtomType* j) const;
	// Generate angle parameters for the supplied UFF atom types
	bool generateAngleTerm(const Species* sp, SpeciesAngle* angleTerm, UFFAtomType* i, UFFAtomType* j, UFFAtomType* k) const;
	// Generate torsion parameters for the supplied UFF atom types
	bool generateTorsionTerm(const Species* sp, SpeciesTorsion* torsionTerm, UFFAtomType* i, UFFAtomType* j, UFFAtomType* k, UFFAtomType* l) const;

	public:
	// Create and assign suitable AtomTypes for the supplied Species
	bool createAtomTypes(Species* sp, CoreData& coreData, bool keepExisting = false) const;
	// Generate intramolecular parameters description for the supplied Species
	bool createIntramolecular(Species* sp, bool useExistingTypes, bool createBonds, bool createAngles, bool createTorsions) const;
	// Perform some test calculations
	void test() const;
};

#endif

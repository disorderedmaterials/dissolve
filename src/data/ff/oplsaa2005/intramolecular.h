/*
	*** OPLSAA (2005) Base Forcefield
	*** src/data/ff/oplsaa2005/base.h
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

#ifndef DISSOLVE_FORCEFIELD_OPLSAA_INTRAMOLECULAR_H
#define DISSOLVE_FORCEFIELD_OPLSAA_INTRAMOLECULAR_H

#include "data/ff.h"

// Forward Declarations
/* none */

// OPLS-AA (2005) Intramolecular Forcefield
class OPLSAA2005IntramolecularTerms : public Forcefield
{
	public:
	// Constructor / Destructor
	OPLSAA2005IntramolecularTerms();
	virtual ~OPLSAA2005IntramolecularTerms();


	/*
	 * Definition
	 */
	public:
	// Return name of Forcefield
	const char* name() const;
	// Return description for Forcefield
	const char* description() const;
	// Return short-range interaction style for AtomTypes
	Forcefield::ShortRangeType shortRangeType() const;


// 	/*
// 	 * Term Data
// 	 */
// 	private:
// 	// Bond terms of the Forcefield
// 	RefList<ForcefieldBondTerm> bondTerms_;
// 	// Angle terms of the Forcefield
// 	RefList<ForcefieldAngleTerm> angleTerms_;
// 	// Torsion terms of the Forcefield
// 	RefList<ForcefieldTorsionTerm> torsionTerms_;
// 	// Improper terms of the Forcefield
// 	RefList<ForcefieldImproperTerm> improperTerms_;
// 
// 	public:
// 	// Return bond term for the supplied atom type pair (if it exists)
// 	virtual ForcefieldBondTerm* bondTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j) const;
// 	// Register specified angle term
// 	void registerAngleTerm(ForcefieldAngleTerm* angleTerm);
// 	// Return angle term for the supplied atom type trio (if it exists)
// 	virtual ForcefieldAngleTerm* angleTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k) const;
// 	// Register specified torsion term
// 	void registerTorsionTerm(ForcefieldTorsionTerm* torsionTerm);
// 	// Return torsion term for the supplied atom type quartet (if it exists)
// 	virtual ForcefieldTorsionTerm* torsionTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const;
// 	// Register specified improper term
// 	void registerImproperTerm(ForcefieldImproperTerm* improperTerm);
// 	// Return improper term for the supplied atom type quartet (if it exists)
// 	virtual ForcefieldImproperTerm* improperTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const;
};

#endif

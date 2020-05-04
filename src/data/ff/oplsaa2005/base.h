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

#ifndef DISSOLVE_FORCEFIELD_OPLSAA_BASE_H
#define DISSOLVE_FORCEFIELD_OPLSAA_BASE_H

#include "data/ff.h"

// Forward Declarations
/* none */

// OPLS-AA (2005) Base Forcefield
class OPLSAA2005BaseForcefield : public Forcefield
{
	public:
	OPLSAA2005BaseForcefield();
	virtual ~OPLSAA2005BaseForcefield();

	/*
	 * Definition
	 */
	public:
	// Return formatted publication references
	const char *publicationReferences() const;
	// Return short-range interaction style for AtomTypes
	Forcefield::ShortRangeType shortRangeType() const;

	/*
	 * Atom Type Data
	 */
	public:
	// Return the base ForcefieldAtomType with specified id (if it exists)
	const ForcefieldAtomType &oplsAtomTypeById(int id) const;

	/*
	 * Term Data
	 */
	public:
	// Return bond term for the supplied atom type pair (if it exists)
	optional<const ForcefieldBondTerm &> bondTerm(const ForcefieldAtomType *i, const ForcefieldAtomType *j) const;
	// Return angle term for the supplied atom type trio (if it exists)
	optional<const ForcefieldAngleTerm &> angleTerm(const ForcefieldAtomType *i, const ForcefieldAtomType *j,
							const ForcefieldAtomType *k) const;
	// Return torsion term for the supplied atom type quartet (if it exists)
	optional<const ForcefieldTorsionTerm &> torsionTerm(const ForcefieldAtomType *i, const ForcefieldAtomType *j,
							    const ForcefieldAtomType *k, const ForcefieldAtomType *l) const;
	// Return improper term for the supplied atom type quartet (if it exists)
	optional<const ForcefieldImproperTerm &> improperTerm(const ForcefieldAtomType *i, const ForcefieldAtomType *j,
							      const ForcefieldAtomType *k, const ForcefieldAtomType *l) const;
};

#endif

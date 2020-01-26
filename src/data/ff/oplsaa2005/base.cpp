/*
	*** OPLSAA (2005) Base Forcefield
	*** src/data/ff/oplsaa2005/base.cpp
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

#include "data/ff/oplsaa2005/base.h"

// Static singletons
OPLSAA2005IntramolecularTerms OPLSAA2005BaseForcefield::intramolecularTerms_;

// Constructor
OPLSAA2005BaseForcefield::OPLSAA2005BaseForcefield()
{
}

OPLSAA2005BaseForcefield::~OPLSAA2005BaseForcefield()
{
}

/*
 * Definition
 */

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType OPLSAA2005BaseForcefield::shortRangeType() const
{
	return Forcefield::LennardJonesGeometricType;
}

/*
 * Term Assignment
 */

// Return bond term for the supplied atom type pair (if it exists)
ForcefieldBondTerm* OPLSAA2005BaseForcefield::bondTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j) const
{
	return intramolecularTerms_.bondTerm(i, j);
}

// Return angle term for the supplied atom type trio (if it exists)
ForcefieldAngleTerm* OPLSAA2005BaseForcefield::angleTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k) const
{
	return intramolecularTerms_.angleTerm(i, j, k);
}

// Return torsion term for the supplied atom type quartet (if it exists)
ForcefieldTorsionTerm* OPLSAA2005BaseForcefield::torsionTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const
{
	return intramolecularTerms_.torsionTerm(i, j, k, l);
}

// Return improper term for the supplied atom type quartet (if it exists)
ForcefieldImproperTerm* OPLSAA2005BaseForcefield::improperTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const
{
	return intramolecularTerms_.improperTerm(i, j, k, l);
}

/*
	*** OPLSAA (2005) Noble Gases Forcefield
	*** src/data/ff/oplsaa2005_noblegases.cpp
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

#include "data/ff/oplsaa2005_noblegases.h"
#include "data/ff/oplsaa2005_atomtypes.h"
#include "data/ffatomtype.h"
#include "classes/speciesatom.h"
#include "base/sysfunc.h"

/*
 * OPLS-AA (2005) Noble Gases
 */

// Constructor / Destructor
Forcefield_OPLSAA2005_NobleGases::Forcefield_OPLSAA2005_NobleGases()
{
	static ForcefieldAtomType atomTypes[] =
	{
		// Copy required types from OPLS-AA (2005) core list
		{ this, "He",	OPLSAA2005_AtomTypes::atomTypeByIndex(101) },
		{ this, "Ne",	OPLSAA2005_AtomTypes::atomTypeByIndex(102) },
		{ this, "Ar",	OPLSAA2005_AtomTypes::atomTypeByIndex(103) },
		{ this, "Kr",	OPLSAA2005_AtomTypes::atomTypeByIndex(104) },
		{ this, "Xe",	OPLSAA2005_AtomTypes::atomTypeByIndex(105) }
	};
}

Forcefield_OPLSAA2005_NobleGases::~Forcefield_OPLSAA2005_NobleGases()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_OPLSAA2005_NobleGases::name() const
{
	return "OPLSAA2005/NobleGases";
}

// Return description for Forcefield
const char* Forcefield_OPLSAA2005_NobleGases::description() const
{
	static CharString desc("Noble gases from OPLS-AA (2005).\n\n%s", OPLSAA2005_AtomTypes::publicationReferences());
	return desc.get();
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_OPLSAA2005_NobleGases::shortRangeType() const
{
	return Forcefield::LennardJonesGeometricType;
}

/*
 * Atom Type Data
 */

// Determine and return atom type for specified SpeciesAtom
ForcefieldAtomType* Forcefield_OPLSAA2005_NobleGases::determineAtomType(SpeciesAtom* i) const
{
	switch (i->element()->Z())
	{
		// Helium
		case (ELEMENT_HE): 
			return atomTypeByName("He", i->element());
			break;
		// Neon
		case (ELEMENT_NE):
			return atomTypeByName("Ne", i->element());
			break;
		// Argon
		case (ELEMENT_AR):
			return atomTypeByName("Ar", i->element());
			break;
		// Krypton
		case (ELEMENT_KR):
			return atomTypeByName("Kr", i->element());
			break;
		// Xenon
		case (ELEMENT_XE):
			return atomTypeByName("Xe", i->element());
			break;
		// Default
		default:
			break;
	}

	return NULL;
}

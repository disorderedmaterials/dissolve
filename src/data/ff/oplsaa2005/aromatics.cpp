/*
	*** OPLSAA (2005) Aromatics Forcefield
	*** src/data/ff/oplsaa2005/aromatics.cpp
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

#include "data/ff/oplsaa2005/aromatics.h"
#include "data/ff/oplsaa2005/atomtypes.h"
#include "data/ffatomtype.h"
#include "classes/speciesatom.h"
#include "base/sysfunc.h"

/*
 * OPLS-AA (2005) Noble Gases
 */

// Constructor / Destructor
Forcefield_OPLSAA2005_Aromatics::Forcefield_OPLSAA2005_Aromatics()
{
	static ForcefieldAtomType atomTypes[] =
	{
		// Copy required types from OPLS-AA (2005) core list
		// -- AA Alkanes
		{ this, "CT",	OPLSAA2005_AtomTypes::atomTypeByIndex(135), "nh=3" },
		{ this, "HC",	OPLSAA2005_AtomTypes::atomTypeByIndex(140), "-&135" },
		// -- Benzene
		{ this, "CA",	OPLSAA2005_AtomTypes::atomTypeByIndex(145), "ring(size=6),-C(n=2),-H(n=1)" },
		{ this, "HA",	OPLSAA2005_AtomTypes::atomTypeByIndex(146), "-&145" },
		// -- Napthalene
		{ this, "CA",	OPLSAA2005_AtomTypes::atomTypeByIndex(147), "ring(size=6,n=2),nbonds=3,-C(n=3)" },
		// -- Toluene
		{ this, "CT",	OPLSAA2005_AtomTypes::atomTypeByIndex(148), "nh=3, -C(ring=6)" },
		// -- Ethylbenzene
		{ this, "CT",	OPLSAA2005_AtomTypes::atomTypeByIndex(149), "nh=2, -C(nh=3), -C(ring(size=6))" }
	};
}

Forcefield_OPLSAA2005_Aromatics::~Forcefield_OPLSAA2005_Aromatics()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_OPLSAA2005_Aromatics::name() const
{
	return "OPLSAA2005/Aromatics";
}

// Return description for Forcefield
const char* Forcefield_OPLSAA2005_Aromatics::description() const
{
	static CharString desc("Aromatics from OPLS-AA (2005), covering benzene, toluene, napthalene, ethylbenzene.<br/><br/>References: %s", OPLSAA2005_AtomTypes::publicationReferences());
	return desc.get();
}

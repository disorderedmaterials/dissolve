/*
	*** OPLSAA (2005) Aromatics Forcefield
	*** src/data/ff/oplsaa2005/aromatics.cpp
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

#include "data/ff/oplsaa2005/aromatics.h"
#include "data/ffatomtype.h"
#include "classes/speciesatom.h"
#include "base/sysfunc.h"

/*
 * OPLS-AA (2005) Aromatics
 */

// Constructor / Destructor
Forcefield_OPLSAA2005_Aromatics::Forcefield_OPLSAA2005_Aromatics()
{
	// Copy required types from OPLS-AA (2005) core list
	// -- AA Alkanes
	copyAtomType(oplsAtomTypeById(135), "CT", "nh=3");
	copyAtomType(oplsAtomTypeById(140), "HC", "-&135");
	// -- Benzene
	copyAtomType(oplsAtomTypeById(145), "CA", "ring(size=6),-C(n=2),-H(n=1)");
	copyAtomType(oplsAtomTypeById(146), "HA", "-&145");
	// -- Napthalene (or larger aromatics)
	copyAtomType(oplsAtomTypeById(147), "CNap", "ring(size=6,n>=2),nbonds=3,-C(n=3)", "CA");
	// -- Toluene
	copyAtomType(oplsAtomTypeById(148), "CT", "nh=3, -C(ring=6)");
	// -- Ethylbenzene
	copyAtomType(oplsAtomTypeById(149), "CT", "nh=2, -C(nh=3), -C(ring(size=6))");
	// -- Phenol
	copyAtomType(oplsAtomTypeById(166), "CA", "nbonds=3,-O(-H),-C(n=2)");
	copyAtomType(oplsAtomTypeById(167), "OH", "-&166");
	copyAtomType(oplsAtomTypeById(168), "HO", "-&167");
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
	static CharString desc("Aromatics from OPLS-AA (2005), covering benzene, toluene, napthalene, ethylbenzene.<br/><br/>References: %s", publicationReferences());
	return desc.get();
}

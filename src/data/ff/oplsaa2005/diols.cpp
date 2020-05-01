/*
	*** OPLSAA (2005) Diols Forcefield
	*** src/data/ff/oplsaa2005/diols.cpp
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

#include "data/ff/oplsaa2005/diols.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"

/*
 * OPLS-AA (2005) Noble Gases
 */

Forcefield_OPLSAA2005_Diols::Forcefield_OPLSAA2005_Diols()
{
	// Copy required types from OPLS-AA (2005) core list
	// -- Diols
	copyAtomType(oplsAtomTypeById(169), "OH", "nh=1,-C(-O(root,-H))");
	copyAtomType(oplsAtomTypeById(170), "HO", "-&169");
	copyAtomType(oplsAtomTypeById(157), "CT", "nh>=2,-O");
	copyAtomType(oplsAtomTypeById(158), "CT", "nh=1,-O");
	copyAtomType(oplsAtomTypeById(159), "CT", "nh=0,-O");
}

Forcefield_OPLSAA2005_Diols::~Forcefield_OPLSAA2005_Diols() {}

/*
 * Definition
 */

// Return name of Forcefield
const char *Forcefield_OPLSAA2005_Diols::name() const { return "OPLSAA2005/Diols"; }

// Return description for Forcefield
const char *Forcefield_OPLSAA2005_Diols::description() const
{
	static CharString desc("Alcohols from OPLS-AA (2005), covering diols only.<br/><br/>References: %s", publicationReferences());
	return desc.get();
}

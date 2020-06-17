/*
	*** OPLSAA (2005) Alkenes Forcefield
	*** src/data/ff/oplsaa2005/alkenes.cpp
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

#include "data/ff/oplsaa2005/alkenes.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "data/ff/oplsaa2005/alkanes.h"
#include "data/ffatomtype.h"

/*
 * OPLS-AA (2005) Noble Gases
 */

// Constructor / Destructor
Forcefield_OPLSAA2005_Alkenes::Forcefield_OPLSAA2005_Alkenes()
{
	// Add required types from OPLS-AA (2005) core list
	// -- AA Alkenes
	copyAtomType(oplsAtomTypeById(141), "CM_a", "nbonds=3,nh=0,-C", "CM");
	copyAtomType(oplsAtomTypeById(142), "CM_a", "nbonds=3,nh=1,-C", "CM");
	copyAtomType(oplsAtomTypeById(143), "CM_a", "nbonds=3,nh=2,-C", "CM");
	copyAtomType(oplsAtomTypeById(144), "HC_d", "-C(nbonds=3)", "HC");
	// -- AA Dienes
	copyAtomType(oplsAtomTypeById(150), "C=", "nbonds=3,nh=1,-C(nh=1,nbonds=3)");
	copyAtomType(oplsAtomTypeById(178), "C=", "nbonds=3,nh=0,-C(nh=0,nbonds=3)");
}

Forcefield_OPLSAA2005_Alkenes::~Forcefield_OPLSAA2005_Alkenes() {}

/*
 * Definition
 */

// Return name of Forcefield
const char *Forcefield_OPLSAA2005_Alkenes::name() const { return "OPLSAA2005/Alkenes"; }

// Return description for Forcefield
const char *Forcefield_OPLSAA2005_Alkenes::description() const
{
	static CharString desc("Alkenes from OPLS-AA (2005), covering alkenes and dienes.<br/><br/>References: %s", publicationReferences());
	return desc.get();
}

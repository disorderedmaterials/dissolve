/*
    *** OPLSAA (2005) Alkanes Forcefield
    *** src/data/ff/oplsaa2005/alkanes.cpp
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

#include "data/ff/oplsaa2005/alkanes.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"

Forcefield_OPLSAA2005_Alkanes::Forcefield_OPLSAA2005_Alkanes() {}

Forcefield_OPLSAA2005_Alkanes::~Forcefield_OPLSAA2005_Alkanes() {}

/*
 * Set Up
 */

// Set up / create all forcefield data ready for use
bool Forcefield_OPLSAA2005_Alkanes::setUp()
{
    // Copy required types from OPLS-AA (2005) core list
    // -- AA Alkanes
    if (!copyAtomType(oplsAtomTypeById(135), "CT3", "nbonds=4,nh=3", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(136), "CT2", "nbonds=4,nh=2", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(137), "CT1", "nbonds=4,nh=1", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(138), "CT4", "nbonds=4,nh=4", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(140), "HC", "-[&135,&136,&137,&138]"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
const char *Forcefield_OPLSAA2005_Alkanes::name() const { return "OPLSAA2005/Alkanes"; }

// Return description for Forcefield
const char *Forcefield_OPLSAA2005_Alkanes::description() const
{
    static CharString desc("Alkanes from OPLS-AA (2005), covering linear and branched alkanes.<br/><br/>References: %s",
                           publicationReferences());
    return desc.get();
}

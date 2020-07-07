/*
    *** OPLSAA (2005) Triols Forcefield
    *** src/data/ff/oplsaa2005/triols.cpp
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

#include "data/ff/oplsaa2005/triols.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "data/ffatomtype.h"

Forcefield_OPLSAA2005_Triols::Forcefield_OPLSAA2005_Triols() {}

Forcefield_OPLSAA2005_Triols::~Forcefield_OPLSAA2005_Triols() {}

/*
 * Set Up
 */

// Set up / create all forcefield data ready for use
bool Forcefield_OPLSAA2005_Triols::setUp()
{
    // Copy required types from OPLS-AA (2005) core list
    // -- Triols
    if (!copyAtomType(oplsAtomTypeById(171), "OH", "nh=1,-C(-O(root,-H))"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(172), "HO", "-&171"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(173), "CT", "nh=2,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(174), "CT", "nh=1,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(175), "CT", "nh=0,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(176), "HC", "-[&173,&174,&175]"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
const char *Forcefield_OPLSAA2005_Triols::name() const { return "OPLSAA2005/Triols"; }

// Return description for Forcefield
const char *Forcefield_OPLSAA2005_Triols::description() const
{
    static CharString desc("Alcohols from OPLS-AA (2005), covering triols only.<br/><br/>References: %s",
                           publicationReferences());
    return desc.get();
}

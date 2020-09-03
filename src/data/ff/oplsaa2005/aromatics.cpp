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

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Aromatics::setUp()
{
    // Copy required types from OPLS-AA (2005) core list
    // -- AA Alkanes
    if (!copyAtomType(oplsAtomTypeById(135), "CT", "nh=3"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(140), "HC", "-&135"))
        return false;
    // -- Benzene
    if (!copyAtomType(oplsAtomTypeById(145), "CA", "ring(size=6),-C(n=2),-H(n=1)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(146), "HA", "-&145"))
        return false;
    // -- Napthalene (or larger aromatics)
    if (!copyAtomType(oplsAtomTypeById(147), "CNap", "ring(size=6,n>=2),nbonds=3,-C(n=3)", "CA"))
        return false;
    // -- Toluene
    if (!copyAtomType(oplsAtomTypeById(148), "CT", "nh=3, -C(ring(size=6))"))
        return false;
    // -- Ethylbenzene
    if (!copyAtomType(oplsAtomTypeById(149), "CT", "nh=2, -C(nh=3), -C(ring(size=6))"))
        return false;
    // -- Phenol
    if (!copyAtomType(oplsAtomTypeById(166), "CA", "nbonds=3,-O(-H),-C(n=2)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(167), "OH", "-&166"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(168), "HO", "-&167"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Aromatics::name() const { return "OPLSAA2005/Aromatics"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Aromatics::description() const
{
    static std::string desc = fmt::format(
        "Aromatics from OPLS-AA (2005), covering benzene, toluene, napthalene, ethylbenzene.<br/><br/>References: {}",
        publicationReferences());

    return desc;
}

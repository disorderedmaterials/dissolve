/*
    *** SPC/Fw Forcefield
    *** src/data/ff/spcfw.cpp
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

#include "data/ff/spcfw.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/speciesatom.h"
#include "data/ffangleterm.h"
#include "data/ffatomtype.h"
#include "data/ffbondterm.h"

/*
 * Implements "Flexible simple point-charge water model with improved liquid-state properties"
 * Yujie Wu, Harald L. Tepper and Gregory A. Voth
 * Journal of Chemical Physics 124 024503 (2006)
 * http://dx.doi.org/10.1063/1.2136877
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * All energy values are in kJ/mol.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_SPCFw::setUp()
{
    // Atom types
    addAtomType(ELEMENT_H, 1, "HW", "-O(nh=2)", "Water hydrogen", 0.41, 0.0, 0.0);
    addAtomType(ELEMENT_O, 2, "OW", "-H(n=2)", "Water oxygen", -0.82, 0.6503, 3.165492);

    // Bond terms
    addBondTerm("HW", "OW", SpeciesBond::HarmonicForm, {4431.53, 1.0});

    // Angle terms
    addAngleTerm("HW", "OW", "HW", SpeciesAngle::HarmonicForm, {317.5656, 113.24});

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_SPCFw::name() const { return "SPC/Fw"; }

// Return description for Forcefield
std::string_view Forcefield_SPCFw::description() const
{
    return "Implements Yujie Wu, Harald L. Tepper and Gregory A. Voth, 'Flexible simple point-charge water model with "
           "improved liquid-state properties', <i>Journal of Chemical Physics</i> "
           "<b>124</b> "
           "024503 (2006), http://dx.doi.org/10.1063/1.2136877";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_SPCFw::shortRangeType() const { return Forcefield::LennardJonesType; }

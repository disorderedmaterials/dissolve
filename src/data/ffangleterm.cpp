/*
    *** Forcefield Angle Term
    *** src/data/ffangleterm.cpp
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

#include "data/ffangleterm.h"
#include "data/ff.h"
#include "data/ffatomtype.h"

ForcefieldAngleTerm::ForcefieldAngleTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                                         SpeciesAngle::AngleFunction form, const std::vector<double> parameters)
{
    typeI_ = typeI;
    typeJ_ = typeJ;
    typeK_ = typeK;
    form_ = form;
    parameters_ = parameters;
    if (!SpeciesAngle::angleFunctions().validNArgs(form, parameters_.size()))
        throw(std::runtime_error("Incorrect number of parameters in constructed ForcefieldAngleTerm."));
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldAngleTerm::isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const
{
    if (!DissolveSys::sameWildString(typeJ_, j.equivalentName()))
        return false;
    if (DissolveSys::sameWildString(typeI_, i.equivalentName()) && DissolveSys::sameWildString(typeK_, k.equivalentName()))
        return true;
    if (DissolveSys::sameWildString(typeK_, i.equivalentName()) && DissolveSys::sameWildString(typeI_, k.equivalentName()))
        return true;

    return false;
}

// Return functional form index of interaction
SpeciesAngle::AngleFunction ForcefieldAngleTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldAngleTerm::parameters() const { return parameters_; }

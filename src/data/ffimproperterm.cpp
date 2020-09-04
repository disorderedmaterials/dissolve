/*
    *** Forcefield Improper Term
    *** src/data/ffimproperterm.cpp
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

#include "data/ffimproperterm.h"
#include "data/ff.h"
#include "data/ffatomtype.h"

ForcefieldImproperTerm::ForcefieldImproperTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                                               std::string_view typeL, SpeciesImproper::ImproperFunction form,
                                               const std::vector<double> parameters)
{
    typeI_ = typeI;
    typeJ_ = typeJ;
    typeK_ = typeK;
    typeL_ = typeL;
    form_ = form;
    parameters_ = parameters;
    if (!SpeciesImproper::improperFunctions().validNArgs(form, parameters_.size()))
        throw(std::runtime_error("Incorrect number of parameters in constructed ForcefieldImproperTerm."));
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldImproperTerm::isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                                     const ForcefieldAtomType &l) const
{
    if (DissolveSys::sameWildString(typeI_, i.equivalentName()) && DissolveSys::sameWildString(typeJ_, j.equivalentName()) &&
        DissolveSys::sameWildString(typeK_, k.equivalentName()) && DissolveSys::sameWildString(typeL_, l.equivalentName()))
        return true;
    if (DissolveSys::sameWildString(typeL_, i.equivalentName()) && DissolveSys::sameWildString(typeK_, j.equivalentName()) &&
        DissolveSys::sameWildString(typeJ_, k.equivalentName()) && DissolveSys::sameWildString(typeI_, l.equivalentName()))
        return true;

    return false;
}

// Return functional form index of interaction
SpeciesImproper::ImproperFunction ForcefieldImproperTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldImproperTerm::parameters() const { return parameters_; }

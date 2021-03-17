// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/angleterm.h"
#include "data/ff/atomtype.h"
#include "data/ff/ff.h"

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

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/angleTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/ff.h"

ForcefieldAngleTerm::ForcefieldAngleTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                                         AngleFunctions::Form form, std::string_view parameterString)
    : typeI_(typeI), typeJ_(typeJ), typeK_(typeK), form_(form)
{
    InteractionPotential<AngleFunctions> potential(form);
    if (!potential.parseParameters(parameterString))
        throw(std::runtime_error(fmt::format("Failed to parse parameter string '{}' when constructing angle term '{}-{}-{}'.\n",
                                             parameterString, typeI_, typeJ_, typeK_)));
    parameters_ = potential.parameters();
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
AngleFunctions::Form ForcefieldAngleTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldAngleTerm::parameters() const { return parameters_; }

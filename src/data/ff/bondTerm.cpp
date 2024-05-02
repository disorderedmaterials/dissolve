// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/bondTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/ff.h"

ForcefieldBondTerm::ForcefieldBondTerm(std::string_view typeI, std::string_view typeJ, BondFunctions::Form form,
                                       std::string_view parameterString)
    : typeI_(typeI), typeJ_(typeJ), form_(form)
{
    InteractionPotential<BondFunctions> potential(form);
    if (!potential.parseParameters(parameterString))
        throw(std::runtime_error(fmt::format("Failed to parse parameter string '{}' when constructing bond term '{}-{}'.\n",
                                             parameterString, typeI_, typeJ_)));
    parameters_ = potential.parameters();
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldBondTerm::isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j) const
{
    if (DissolveSys::sameWildString(typeI_, i.equivalentName()) && DissolveSys::sameWildString(typeJ_, j.equivalentName()))
        return true;
    if (DissolveSys::sameWildString(typeJ_, i.equivalentName()) && DissolveSys::sameWildString(typeI_, j.equivalentName()))
        return true;

    return false;
}

// Return functional form index of interaction
BondFunctions::Form ForcefieldBondTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldBondTerm::parameters() const { return parameters_; }

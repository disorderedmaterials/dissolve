// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/improperTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/ff.h"

ForcefieldImproperTerm::ForcefieldImproperTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                                               std::string_view typeL, TorsionFunctions::Form form,
                                               std::string_view parameterString)
    : typeI_(typeI), typeJ_(typeJ), typeK_(typeK), typeL_(typeL), form_(form)
{
    InteractionPotential<TorsionFunctions> potential(form);
    if (!potential.parseParameters(parameterString))
        throw(std::runtime_error(
            fmt::format("Failed to parse parameter string '{}' when constructing improper term '{}-{}-{}-{}'.\n",
                        parameterString, typeI_, typeJ_, typeK_, typeL_)));
    parameters_ = potential.parameters();
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldImproperTerm::isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                                     const ForcefieldAtomType &l) const
{
    // 'i' is our central atom, so 'j', 'k', and 'l' can be specified in any order
    if (!DissolveSys::sameWildString(typeI_, i.equivalentName()))
        return false;
    std::vector<std::string> typeNames = {typeJ_, typeK_, typeL_};
    for (auto n = 0; n < 3; ++n)
    {
        // Check for jkl or lkj matches
        if (DissolveSys::sameWildString(typeNames[1], k.equivalentName()))
        {

            if (DissolveSys::sameWildString(typeNames[0], j.equivalentName()) &&
                DissolveSys::sameWildString(typeNames[2], l.equivalentName()))
                return true;
            if (DissolveSys::sameWildString(typeNames[2], j.equivalentName()) &&
                DissolveSys::sameWildString(typeNames[0], l.equivalentName()))
                return true;
        }

        // Shuffle the names
        typeNames.push_back(typeNames[0]);
        typeNames.erase(typeNames.begin());
    }

    return false;
}

// Return functional form index of interaction
TorsionFunctions::Form ForcefieldImproperTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldImproperTerm::parameters() const { return parameters_; }

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/torsionTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/ff.h"

ForcefieldTorsionTerm::ForcefieldTorsionTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                                             std::string_view typeL, TorsionFunctions::Form form,
                                             std::string_view parameterString, double q14Scale, double vdw14Scale)
    : typeI_(typeI), typeJ_(typeJ), typeK_(typeK), typeL_(typeL), form_(form), q14Scale_(q14Scale), vdw14Scale_(vdw14Scale)
{
    InteractionPotential<TorsionFunctions> potential(form);
    if (!potential.parseParameters(parameterString))
        throw(std::runtime_error(
            fmt::format("Failed to parse parameter string '{}' when constructing torsion term '{}-{}-{}-{}'.\n",
                        parameterString, typeI_, typeJ_, typeK_, typeL_)));
    parameters_ = potential.parameters();
}

/*
 * Data
 */

// Return if this term matches the atom types supplied
bool ForcefieldTorsionTerm::isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
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
TorsionFunctions::Form ForcefieldTorsionTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldTorsionTerm::parameters() const { return parameters_; }

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/torsionterm.h"
#include "data/ff/atomtype.h"
#include "data/ff/ff.h"

ForcefieldTorsionTerm::ForcefieldTorsionTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                                             std::string_view typeL, SpeciesTorsion::TorsionFunction form,
                                             const std::vector<double> parameters)
{
    typeI_ = typeI;
    typeJ_ = typeJ;
    typeK_ = typeK;
    typeL_ = typeL;
    form_ = form;
    parameters_ = parameters;
    if (!SpeciesTorsion::torsionFunctions().validNArgs(form, parameters_.size()))
        throw(std::runtime_error("Incorrect number of parameters in constructed ForcefieldTorsionTerm."));
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
SpeciesTorsion::TorsionFunction ForcefieldTorsionTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldTorsionTerm::parameters() const { return parameters_; }

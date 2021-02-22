// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/bondterm.h"
#include "data/ff/atomtype.h"
#include "data/ff/ff.h"

ForcefieldBondTerm::ForcefieldBondTerm(std::string_view typeI, std::string_view typeJ, SpeciesBond::BondFunction form,
                                       const std::vector<double> parameters)
{
    typeI_ = typeI;
    typeJ_ = typeJ;
    form_ = form;
    parameters_ = parameters;
    if (!SpeciesBond::bondFunctions().validNArgs(form, parameters_.size()))
        throw(std::runtime_error("Incorrect number of parameters in constructed ForcefieldBondTerm."));
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
SpeciesBond::BondFunction ForcefieldBondTerm::form() const { return form_; }

// Return array of parameters
const std::vector<double> &ForcefieldBondTerm::parameters() const { return parameters_; }

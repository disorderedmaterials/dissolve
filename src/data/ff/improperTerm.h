// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesImproper.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Improper Term
class ForcefieldImproperTerm
{
    public:
    ForcefieldImproperTerm(std::string_view typeI = "", std::string_view typeJ = "", std::string_view typeK = "",
                           std::string_view typeL = "", TorsionFunctions::Form form = TorsionFunctions::Form::None,
                           std::string_view parameterString = "");
    ~ForcefieldImproperTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_, typeK_, typeL_;
    // Functional form of interaction
    TorsionFunctions::Form form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                 const ForcefieldAtomType &l) const;
    // Return functional form index of interaction
    TorsionFunctions::Form form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};

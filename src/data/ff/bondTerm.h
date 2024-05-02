// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesBond.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Bond Term
class ForcefieldBondTerm
{
    public:
    ForcefieldBondTerm(std::string_view typeI = "", std::string_view typeJ = "",
                       BondFunctions::Form form = BondFunctions::Form::None, std::string_view parameterString = "");
    ~ForcefieldBondTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_;
    // Functional form of interaction
    BondFunctions::Form form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j) const;
    // Return functional form index of interaction
    BondFunctions::Form form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};

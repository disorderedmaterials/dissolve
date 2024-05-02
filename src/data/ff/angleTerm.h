// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAngle.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Angle Term
class ForcefieldAngleTerm
{
    public:
    ForcefieldAngleTerm(std::string_view typeI = "", std::string_view typeJ = "", std::string_view typeK = "",
                        AngleFunctions::Form form = AngleFunctions::Form::None, std::string_view parameterString = "");
    ~ForcefieldAngleTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_, typeK_;
    // Functional form of interaction
    AngleFunctions::Form form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const;
    // Return functional form index of interaction
    AngleFunctions::Form form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesangle.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Angle Term
class ForcefieldAngleTerm
{
    public:
    ForcefieldAngleTerm(std::string_view typeI = "", std::string_view typeJ = "", std::string_view typeK = "",
                        SpeciesAngle::AngleFunction form = SpeciesAngle::NoForm, const std::vector<double> parameters = {});
    ~ForcefieldAngleTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_, typeK_;
    // Functional form of interaction
    SpeciesAngle::AngleFunction form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const;
    // Return functional form index of interaction
    SpeciesAngle::AngleFunction form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};
